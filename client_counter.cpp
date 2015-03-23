#include <NTL/ZZ.h>
#include "FHE.h"
#include "timing.h"
#include "EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sstream>
#include <iostream>
#include <string>

using namespace std;
FHEcontext* context;
FHESecKey* secretKey;
FHEPubKey* publicKey;
ZZX G;

void setupHE();
void error(const char* msg);
void initCounter();

int main(int argc, char *argv[])
{   
  int people = 3;
  int sockfd, portno, bytes_read, bytes_written;
  int miniBufferSize = 256, responseBufferSize;

  struct sockaddr_in serv_addr;
  struct hostent *server;

  setupHE();
  
  if (argc < 3) {
    fprintf(stderr,"usage %s hostname port\n", argv[0]);
    exit(0);
  }
  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (sockfd < 0) 
    error("ERROR opening socket");
  server = gethostbyname(argv[1]);
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    error("ERROR connecting");

  /** Initializing ciphertext **/

  EncryptedArray ea(*context, G);
  PlaintextArray counter(ea);  
  counter.encode(people);
  Ctxt& encryptedCounter = *(new Ctxt(*publicKey));  
  ea.encrypt(encryptedCounter, *publicKey, counter);
 

  ostringstream oss;
  oss << encryptedCounter;

  /** Send ciphertext to server **/
  // First sends the length
  
  int msgsize = oss.str().length(); 
  ostringstream msglen;
  msglen << msgsize;
  cout << "Sending ciphertext size = " << msglen.str() << endl;
  bytes_written = write(sockfd, msglen.str().c_str(), miniBufferSize);
  if (bytes_written < 0)
    error("ERROR writing to socket");
  
  
  // Then sends the whole ciphertext
  bytes_written = write(sockfd, oss.str().c_str(), msgsize);
  if (bytes_written < 0) 
    error("ERROR writing to socket");
  
  /** Read ciphertext from server **/
  /*
  // First reads the length
  char* miniBuffer = new char[miniBufferSize];
  bzero(miniBuffer, miniBufferSize);
  bytes_read = read(sockfd, miniBuffer, miniBufferSize);
  if (bytes_read < 0)
      error("ERROR reading from socket");
  responseBufferSize = atoi(miniBuffer);

  // Then reads the whole ciphertext
  char* responseBuffer = new char[responseBufferSize];
  bzero(responseBuffer, responseBufferSize);
  bytes_read = read(sockfd, responseBuffer, responseBufferSize);
  if (bytes_read < 0) 
    error("ERROR reading from socket");


  Ctxt& receivedCipher = *(new Ctxt(*publicKey));  
  istringstream istream;
  istream.str(responseBuffer);

  istream >> receivedCipher;
  
  cout << "Printing decrypted ciphertext after reading it..." << endl;
  PlaintextArray decryptedStream(ea);
  ea.decrypt(receivedCipher, *secretKey, decryptedStream);
  decryptedStream.print(cout);
  */

  close(sockfd);
  return 0;
}

void initCounter() {
  return;
}

void error(const char *msg){
    perror(msg);
    exit(0);
}

void setupHE(){
  long R=1;
  long p=101;
  long r=1;
  long L=0;
  long c=2;
  long k=80;
  long s=0;
  long d=1;
  long w=64;
  
  if (L==0) { // determine L based on R,r
    L = 3*R+3;
    if (p>2 || r>1) { // add some more primes for each round
      long addPerRound = 2*ceil(log((double)p)*r*3)/(log(2.0)*FHE_p2Size) +1;
      L += R * addPerRound;
    }
  }
  long m = FindM(k, L, c, p, d, s, 0);

  cerr << "\n\n******** Printing parameters: R=" << R 
       << ", p=" << p
       << ", r=" << r
       << ", d=" << d
       << ", c=" << c
       << ", k=" << k
       << ", w=" << w
       << ", L=" << L
       << ", m=" << m
       << endl;
  
  context = new FHEcontext(m,p,r);
  buildModChain(*context, L, c);
    
  if (d == 0)
    G = context->alMod.getFactorsOverZZ()[0];
  else
    G = makeIrredPoly(p, d); 

  secretKey = new FHESecKey(*context);
  publicKey = secretKey;

  secretKey->GenSecKey(w); 
  addSome1DMatrices(*secretKey); // compute key-switching matrices that we need
  return;
}
