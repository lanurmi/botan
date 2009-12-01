/*
  Implement the functionality of a simple CA: read in a CA certificate,
  the associated private key, and a PKCS #10 certificate request. Sign the
  request and print out the new certificate.

  File names are hardcoded for simplicity.
    cacert.pem:    The CA's certificate (perhaps created by self_sig)
    caprivate.pem: The CA's private key
    req.pem:       The user's PKCS #10 certificate request

  Written by Jack Lloyd, May 19, 2003

  This file is in the public domain.
*/

#include <botan/botan.h>
#include <botan/x509_ca.h>
#include <botan/time.h>
using namespace Botan;

#include <iostream>
#include <memory>

int main(int argc, char* argv[])
   {
   if(argc != 5)
      {
      std::cout << "Usage: " << argv[0] << " <passphrase> "
                << "<ca cert> <ca key> <pkcs10>" << std::endl;
      return 1;
      }

   Botan::LibraryInitializer init;

   try
      {
      const std::string arg_passphrase = argv[1];
      const std::string arg_ca_cert = argv[2];
      const std::string arg_ca_key = argv[3];
      const std::string arg_req_file = argv[4];

      AutoSeeded_RNG rng;

      X509_Certificate ca_cert(arg_ca_cert);

      std::auto_ptr<PKCS8_PrivateKey> privkey(
         PKCS8::load_key(arg_ca_key, rng, arg_passphrase)
         );

      X509_CA ca(ca_cert, *privkey, "SHA-256");

      // got a request
      PKCS10_Request req(arg_req_file);

      // you would insert checks here, and perhaps modify the request
      // (this example should be extended to show how)

      // now sign the request
      auto now = std::chrono::system_clock::now();

      X509_Time start_time(now);

      typedef std::chrono::duration<int, std::ratio<31556926>> years;

      X509_Time end_time(now + years(1));

      X509_Certificate new_cert = ca.sign_request(req, rng,
                                                  start_time, end_time);

      // send the new cert back to the requestor
      std::cout << new_cert.PEM_encode();
   }
   catch(std::exception& e)
      {
      std::cout << e.what() << std::endl;
      return 1;
      }
   return 0;
   }
