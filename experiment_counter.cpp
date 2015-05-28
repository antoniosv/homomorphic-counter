#include <NTL/ZZ.h>
#include "FHE.h"
#include "timing.h"
#include "EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>

#include <cassert>
#include <cstdio>
#include <sstream>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>


using namespace std;

const int numTests = 2;
const int tratamientos = 4;
FHEcontext* context;
FHESecKey* secretKey;
FHEPubKey* publicKey;
ZZX G;
EncryptedArray* ea;
long K[tratamientos] = {40, 60, 80, 100}; 
double genKeyTime[tratamientos][numTests];
double encryptionTime[tratamientos][numTests];
double decryptionTime[tratamientos][numTests];
double additionTime[tratamientos][numTests];
int keySize[tratamientos][numTests];

void csvwriter() {
  ofstream keyData, keySizeData, encryptData, addData, decryptData;
  keyData.open("experiments/keygen.csv");
  // keySizeData.open("experiments/keysize.csv");
  // encryptData.open("experiments/encrypt.csv");
  // addData.open("experiments/add.csv");
  // decryptData.open("experiments/decrypt.csv"); 

  for(int i=0;i<tratamientos; i++) {
    keyData << K[i] << ",";
    // keySizeData << K[i] << ",";
    // encryptData << K[i] << ",";
    // addData << K[i] << ",";
    // decryptData << K[i] << ",";    
    for(int j=0; j<numTests; j++) {
      keyData << genKeyTime[i][j] << ",";
      // keySizeData << keySize[i][j] << ",";
      // encryptData << encryptionTime[i][j] << ",";
      // addData << additionTime[i][j] << ",";
      // decryptData << decryptionTime[i][j] << ",";
    }
     keyData << "\n";
  }
  
  keyData.close(); //encryptData.close(); addData.close(); decryptData.close();
  
}

void  setUp(long R, long p, long r, long d, long c, long k, long w, 
               long L, long m, const Vec<long>& gens, const Vec<long>& ords)
{
  context = new FHEcontext(m, p, r);
  buildModChain(*context, L, c);

  if (d == 0)
    G = context->alMod.getFactorsOverZZ()[0];
  else
    G = makeIrredPoly(p, d); 

  secretKey = new FHESecKey(*context);
  publicKey = secretKey;

  secretKey->GenSecKey(w);

  addSome1DMatrices(*secretKey); // compute key-switching matrices that we need  
}

string encryption()
{
  ostringstream oss;
  ea = new EncryptedArray(*context, G);
  // Plaintext encoding
  PlaintextArray counter(*ea);
  counter.encode(50);
  Ctxt encryptedCounter(*publicKey);  
  ea->encrypt(encryptedCounter, *publicKey, counter);
  oss << encryptedCounter;
  return oss.str();
}

void addition(string str)
{
  istringstream iss;
  iss.str(str);
  Ctxt encryptedCounter(*publicKey);  
  iss >> encryptedCounter;

  PlaintextArray p0(*ea);
  p0.random();
  Ctxt c0(*publicKey);
  ea->encrypt(c0, *publicKey, p0);

  encryptedCounter += c0;
  return;
}

void decryption(string str)
{
  istringstream iss;
  iss.str(str);
  Ctxt encryptedCounter(*publicKey);  
  iss >> encryptedCounter;
  // decrypting ciphertext
  PlaintextArray decryptedCounter(*ea);
  ea->decrypt(encryptedCounter, *secretKey, decryptedCounter);
  // cout << "Modified plaintext: "; 
  // decryptedCounter.print(cout);
}

int publicKeySize() {
  int size;
  {fstream keyFile("iotest.txt", ios::binary | ios::ate | fstream::out|fstream::trunc);
    keyFile << *publicKey << endl;
    size = keyFile.tellg();
    keyFile.flush();
    keyFile.close();}

  return size;
}

/* A general test program that uses a mix of operations over four ciphertexts.
 * Usage: Test_General_x [ name=value ]...
 *   R       number of rounds  [ default=1 ]
 *   p       plaintext base  [ default=2 ]
 *   r       lifting  [ default=1 ]
 *   d       degree of the field extension  [ default=1 ]
 *              d == 0 => factors[0] defines extension
 *   c       number of columns in the key-switching matrices  [ default=2 ]
 *   k       security parameter  [ default=80 ]
 *   L       # of levels in the modulus chain  [ default=heuristic ]
 *   s       minimum number of slots  [ default=0 ]
 *   repeat  number of times to repeat the test  [ default=1 ]
 *   m       use specified value as modulus
 *   mvec    use product of the integers as  modulus
 *              e.g., mvec='[5 3 187]' (this overwrite the m argument)
 *   gens    use specified vector of generators
 *              e.g., gens='[562 1871 751]'
 *   ords    use specified vector of orders
 *              e.g., ords='[4 2 -4]', negative means 'bad'
 */
int main(int argc, char **argv) 
{
  /*
    1. Establecer posibles valores para parametros (K)
    2. Hacer un doble loop 
    3. En en loop exterior itera los valores de los parametros
    4. En el loop interior itera corridas de encriptacion/generacion de clave publica
    5. Al iniciar el loop interior corre el tiempo, y termina al acabar esa iteracion
    6. Guarda en un csv los valores de K, y los tiempos capturados 
  */

  long R=1;
  long p=101;
  long r=1;
  long d=1;
  long c=2;
  long k=20;
  long L=0;
  long s=0;
  //  long repeat=1;
  long chosen_m=0;
  Vec<long> mvec;
  Vec<long> gens;
  Vec<long> ords;
  
  if (L==0) { // determine L based on R,r
    L = 3*R+3;
    if (p>2 || r>1) { // add some more primes for each round
      long addPerRound = 2*ceil(log((double)p)*r*3)/(log(2.0)*FHE_p2Size) +1;
      L += R * addPerRound;
    }
  }

  long w = 64; // Hamming weight of secret key
  //  long L = z*R; // number of levels

  if (mvec.length()>0)
    chosen_m = computeProd(mvec);
  long m;
  clock_t start;
  double duration;  
  for (int t = 0; t < tratamientos; t++) {    
    k = K[t];
    for (int rep = 0; rep<numTests; rep++) {
      cout << "Iteration no. " << rep << endl;
      cout << "Generating public and private keys" << endl;
      start = clock();
      m = FindM(k, L, c, p, d, s, chosen_m, true);      
      setUp(R, p, r, d, c, k, w, L, m, gens, ords);
      duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
      genKeyTime[t][rep] = duration;

      keySize[t][rep] = publicKeySize();    

      cout << "Encrypting plaintext" << endl;
      start = clock();
      string cipherStr=encryption();
      duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
      encryptionTime[t][rep] = duration;
      
      cout << "Performing addition" << endl;
      start = clock();
      addition(cipherStr);
      duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
      additionTime[t][rep] = duration;

      cout << "Decrypting ciphertext" << endl;
      start = clock();
      decryption(cipherStr);
      duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
      decryptionTime[t][rep] = duration;       

    }
  }

  for(int i=0; i<tratamientos; i++) {
    for(int j=0; j<numTests; j++) {
      cout << "Keygen: " << genKeyTime[i][j] << "\tKeysize: " << keySize[i][j] << "\tEncrypt: " << encryptionTime[i][j] << "\tAdd: " << additionTime[i][j] << "\tDecrypt: " << decryptionTime[i][j] << endl;
    }
  }

  csvwriter();
}
