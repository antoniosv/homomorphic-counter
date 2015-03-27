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

int sendalldata(int s, char const* buffer, int *len);
void error(const char* msg);
int openSocket();

void writeToFile(istringstream& in) {
  string modified = in.str();
  ofstream myfile;
  myfile.open("/home/jesus/homomorphic-counter/second.txt");
  myfile << modified;
  myfile.close();
  cout << "writing to file" << endl;
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
  bytes_read = read(newsockfd, miniBuffer, miniBufferSize);

  if (bytes_read < 0) error("ERROR reading from socket");
  responseBufferSize = atoi(miniBuffer);

  cout << "Mini buffer size received... " << responseBufferSize << endl;
  

  // Then reads the whole ciphertext

  char* responseBuffer = new char[responseBufferSize];
  bzero(responseBuffer, responseBufferSize);
  bytes_read = 0;
  int bytes_remaining = responseBufferSize;
  int this_recv;
  //  setsockopt(newsockfd, SOL_SOCKET, SO_RCVTIMEO, ...);
  while(bytes_remaining > 0) {    
    this_recv = recv(newsockfd, responseBuffer+bytes_read, bytes_remaining, 0); 
    if(this_recv <=0) error("error on receive");
    else {
      bytes_remaining -= this_recv;
      bytes_read += this_recv;
    }
  }

  string strBuffer((const char*) responseBuffer, bytes_read);
  istringstream istream;
  istream.str(strBuffer);
  writeToFile(istream);

  cout << "Cipher buffer received... " << istream.str().size() << endl;

    
  // reconstructing ciphertext 
  /*
  FHEPubKey& publicKey; // & despues de FHEPubKey
  Ctxt encryptedCounter(publicKey);
  Ctxt encryptedQty(publicKey);
  */
    
  // operations on ciphertext
  //  printf("Here is the message: %s\n", buffer); // %s

  
  //  (encryptedCounter)+=(encryptedQty);
  //  (encryptedCounter)-=encryptedQty;

  // serialize modified ciphertext
   
  //  cout << "Serializing ciphertext object..." << endl;
  
   ostringstream oss;
  // //  ostream << encryptedCounter;
   oss.str(istream.str());
  
   /* sends the ciphertext it back to client */
   // First sends back the size
   int msgsize = oss.str().length();
   ostringstream msglen;
   msglen << msgsize;
   cout << "Sending ciphertext size = " << msglen.str() << endl;
   bytes_written = write(newsockfd, msglen.str().c_str(), miniBufferSize);
   if(bytes_written < 0)
     error("ERROR writing to socket");
   
  
  //  Ctxt receivedCipher(publicKey);
  //  serialCipher.str(receivedCipher);

   // Then sends the whole ciphertext
  try {
    if(sendalldata(newsockfd, oss.str().c_str(), &msgsize) == -1) {
      cout << "Se enviaron solo " << msgsize << " bytes por el error" << endl;
    }
    else
      cout << "se enviaron los " << msgsize << " bytes del mensaje" << endl;
  	//bytes_written = write(newsockfd, oss.str().c_str(), msglen);	
  	//    if(bytes_written <0) error ("ERROR writing to socket");
  } catch(int e) {
    cout << "Exception has ocurred... Exception no. " << e << endl;
  }
  
  cout << "closing sockets..." << endl;
  close(newsockfd);
  close(sock);

  return 0;   
}

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

int sendalldata(int s, char const* buffer,  int *len)
{
  int total = 0; // bytes sent
  int bytesleft = *len; // bytes left to send
  int n;
  
  while(total < *len) {
    n = send(s, buffer+total, bytesleft, 0);
    if (n == -1) { break; }
    total += n;
    bytesleft -= n;
  }

  *len = total; 

  if(n == -1) 
    return -1;
  else 
    return 0;
}
