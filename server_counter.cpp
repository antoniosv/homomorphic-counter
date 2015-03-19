#include <NTL/ZZ.h>
#include "FHE.h"
#include "timing.h"
#include "EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>

#include <cassert>
#include <cstdio>
#include <sstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


#ifdef DEBUG
#define debugCompare(ea,sk,p,c) {\
  PlaintextArray pp(ea);\
  ea.decrypt(c, sk, pp);\
  if (!pp.equals(p)) { cerr << "oops\n"; exit(0); }\
  }
#else
#define debugCompare(ea,sk,p,c)
#endif


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int openSocket() {
  int sockfd;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) 
       error("ERROR opening socket");
  return sockfd;

}

int main(int argc, char *argv[]){
  /**  TODO
       Do the same as the client
       1. Receive buffer size
       2. Receive full buffer
       3. Send buffer size
       4. Send full buffer
   **/
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;  
  int sock, newsockfd, portno;
  int bytes_read, bytes_written;
  int  miniBufferSize = 256, responseBufferSize;

  
  // opens up socket
   
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  } 
  
  sock = openSocket();
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  // binds sicket to port no. specified
  if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    error("ERROR on binding");
  listen(sock, 5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sock, (struct sockaddr *) &cli_addr, &clilen);
  if(newsockfd < 0)
    error("ERROR ON ACCEPT");

  /* Receive serialized ciphertext from client */

  // First reads the length
  
  char* miniBuffer = new char[miniBufferSize];
  bzero(miniBuffer, miniBufferSize);
  bytes_read = read(newsockfd, miniBuffer, 256);

  if (bytes_read < 0) error("ERROR reading from socket");
  responseBufferSize = atoi(miniBuffer);

  cout << "Mini buffer size received... " << miniBuffer << endl;
  
  /*
  // Then reads the whole ciphertext
  char* responseBuffer = new char[responseBufferSize];
  bzero(responseBuffer, responseBufferSize);
  bytes_read = -1;
  bytes_read = read(newsockfd, responseBuffer, responseBufferSize);
  if (bytes_read < 0) 
    error("ERROR reading from socket");

  string strBuffer((const char*) responseBuffer, bytes_read);
  istringstream istream;
  istream.str(strBuffer);
  cout << "Cipher buffer received... " << istream.str().size() << endl;
  */

    
  // reconstructing ciphertext
  /*  
  FHEPubKey publicKey; // & despues de FHEPubKey
  Ctxt encryptedCounter(publicKey);
  Ctxt encryptedQty(publicKey);
  */

    
  // operations on ciphertext
  //  printf("Here is the message: %s\n", buffer); // %s

  
  //  (encryptedCounter)+=(encryptedQty);
  //  (encryptedCounter)-=encryptedQty;

  // serialize modified ciphertext
   
  //  cout << "Serializing ciphertext object..." << endl;
  /*  
  ostringstream oss;
  //  ostream << encryptedCounter;
  oss.str(istream.str());
  int msglen = strlen(oss.str().c_str());

  
  // send it back to client
  
  //  Ctxt receivedCipher(publicKey);
  //  serialCipher.str(receivedCipher);

  //  cout << "Sending back... " << oss.str() << endl;

  try {
    bytes_written = write(newsockfd, oss.str().c_str(), msglen);
    //  bytes_written = write(newsockfd, "I gots your message", 18);
    if(bytes_written <0) error ("ERROR writing to socket");
  } catch(int e) {
    cout << "Exception has ocurred... Exception no. " << e << endl;
  }
  */
  
  cout << "closing sockets..." << endl;
  close(newsockfd);
  close(sock);

  return 0; 

  
}

