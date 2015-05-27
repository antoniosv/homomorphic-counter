#include <NTL/ZZ.h>
#include "FHE.h"
#include "timing.h"
#include "EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>

#include <cassert>
#include <cstdio>
#include <sstream>
#include <ctime>


using namespace std;

const int numTests = 5;
const int paramLoops = 10;
FHEcontext* context;
FHESecKey* secretKey;
FHEPubKey* publicKey;
ZZX G;
EncryptedArray* ea;
double* genKeyTime[paramLoops][numTests];

//  setTimersOn();

void  setUp(long R, long p, long r, long d, long c, long k, long w, 
               long L, long m, const Vec<long>& gens, const Vec<long>& ords)
{
  context = new FHEcontext(m, p, r);
  buildModChain(*context, L, c);

  cout << "aaa" << endl;
  if (d == 0)
    G = context->alMod.getFactorsOverZZ()[0];
  else
    G = makeIrredPoly(p, d); 

  secretKey = new FHESecKey(*context);
  publicKey = secretKey;

  addSome1DMatrices(*secretKey); // compute key-switching matrices that we need  
}

void encryption()
{
  ea = new EncryptedArray(*context, G);
  // Plaintext encoding
  PlaintextArray counter(*ea);  
  counter.encode(50);
  Ctxt encryptedCounter(*publicKey);  
  ea->encrypt(encryptedCounter, *publicKey, counter);

}

void decryption(Ctxt encryptedCounter)
{
  // decrypting ciphertext
  PlaintextArray decryptedCounter(*ea);
  ea->decrypt(encryptedCounter, *secretKey, decryptedCounter);
  cout << "Modified plaintext: ";
}

// void serialize(Ctxt encryptedCounter)
// {   
//   cout << "Serializing ciphertext object..." << endl;
//   ostringstream ostream;
//   ostream << encryptedCounter;
  
//   Ctxt receivedCipher(*publicKey);
//   //  serialCipher.str(receivedCipher);
//   istringstream istream;
//   istream.str(ostream.str());
//   istream >> receivedCipher;

//   cout << "Printing decrypted ciphertext after reading it..." << endl;
//   PlaintextArray decryptedStream(ea);
//   ea.decrypt(receivedCipher, *secretKey, decryptedStream);
//   decryptedStream.print(cout); 
// }

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
  long K[numTests] = {40, 60, 80, 100, 120}; 

  long R=1;
  long p=101;
  long r=1;
  long d=1;
  long c=2;
  long k=80;
  long L=0;
  long s=0;
  long repeat=1;
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
  for (int kCount = 0; kCount<numTests; kCount++) {
    for (long repeat_cnt = 0; repeat_cnt < repeat; repeat_cnt++) {
      cout << kCount << endl;
      //start = clock();
      m = FindM(k, L, c, p, d, s, chosen_m, true);      
      setUp(R, p, r, d, c, k, w, L, m, gens, ords);
      //      duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
      //cout<<"printf: "<< duration <<'\n';
    }
  }
}
