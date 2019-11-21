//
// Created by jin on 11/21/19.
//
#include <gsoap/plugin/wsseapi.h>
#include "helpers.h"
#include "config.h"

void set_credentials(struct soap *soap)
{
    soap_wsse_delete_Security(soap);
    if (soap_wsse_add_Timestamp(soap, "Time", 10)
        || soap_wsse_add_UsernameTokenDigest(soap, "Auth", USERNAME, PASSWORD))
        report_error(soap);
#ifdef PROTECT
    if (!privk)
  {
    FILE *fd = fopen("client.pem";
    if (fd)
    {
      privk = PEM_read_PrivateKey(fd, NULL, NULL, (void*)"password");
      fclose(fd);
    }
    if (!privk)
    {
      fprintf(stderr, "Could not read private key from client.pem\n");
      exit(EXIT_FAILURE);
    }
  }
  if (!cert)
  {
    FILE *fd = fopen("clientcert.pem", "r");
    if (fd)
    {
      cert = PEM_read_X509(fd, NULL, NULL, NULL);
      fclose(fd);
    }
    if (!cert)
    {
      fprintf(stderr, "Could not read certificate from clientcert.pem\n");
      exit(EXIT_FAILURE);
    }
  }
  if (soap_wsse_add_BinarySecurityTokenX509(soap, "X509Token", cert)
   || soap_wsse_add_KeyInfo_SecurityTokenReferenceX509(soap, "#X509Token")
   || soap_wsse_sign_body(soap, SOAP_SMD_SIGN_RSA_SHA256, rsa_privk, 0)
   || soap_wsse_verify_auto(soap, SOAP_SMD_NONE, NULL, 0))
    report_error(soap);
#endif
}

void report_error(struct soap *soap)
{
    std::cerr << "Oops, something went wrong:" << std::endl;
    soap_stream_fault(soap, std::cerr);
    exit(EXIT_FAILURE);
}