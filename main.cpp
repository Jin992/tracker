#include "onvif/soapDeviceBindingProxy.h"
#include "onvif/soapMediaBindingProxy.h"
#include "onvif/soapPTZBindingProxy.h"
#include "onvif/soapPullPointSubscriptionBindingProxy.h"
#include "onvif/soapRemoteDiscoveryBindingProxy.h"
#include "plugin/wsddapi.h"
#include "plugin/wsseapi.h"
#include "onvif/wsdd.nsmap"
#include "config.h"
#include "services/DeviceService.h"
#include "helpers.h"
#include "services/MediaService.h"

// using http instead of https is not safe unless you secure message integrity with WS-Security by uncommenting:
// #define PROTECT

#ifdef PROTECT
// define some global data that is set once, to keep this example simple
EVP_PKEY *privk = NULL;
X509 *cert = NULL;
#endif

// to check if an ONVIF service response was signed with WS-Security (when enabled)
void check_response(struct soap *soap)
{
#ifdef PROTECT
  // check if the server returned a signed message body, if not error
  if (soap_wsse_verify_body(soap))
    report_error(soap);
  soap_wsse_delete_Security(soap);
#endif
}

// to download a snapshot and save it locally in the current dir as image-1.jpg, image-2.jpg, image-3.jpg ...
void save_snapshot(int i, const char *endpoint)
{
  char filename[32];
  (SOAP_SNPRINTF_SAFE(filename, 32), "image-%d.jpg", i);
  FILE *fd = fopen(filename, "wb");
  if (!fd)
  {
    std::cerr << "Cannot open " << filename << " for writing" << std::endl;
    exit(EXIT_FAILURE);
  }

  // create a temporary context to retrieve the image with HTTP GET
  struct soap *soap = soap_new();
  soap->connect_timeout = soap->recv_timeout = soap->send_timeout = 10; // 10 sec

  // HTTP GET and save image
  if (soap_GET(soap, endpoint, NULL) || soap_begin_recv(soap))
    report_error(soap);
  std::cout << "Retrieving " << filename;
  if (soap->http_content)
    std::cout << " of type " << soap->http_content;
  std::cout << " from " << endpoint << std::endl;

  // this example stores the whole image in memory first, before saving it to the file
  // better is to copy the source code of soap_http_get_body here and
  // modify it to save data directly to the file.
  size_t imagelen;
  char *image = soap_http_get_body(soap, &imagelen); // NOTE: soap_http_get_body was renamed from soap_get_http_body in gSOAP 2.8.73
  soap_end_recv(soap);
  fwrite(image, 1, imagelen, fd);
  fclose(fd);

  //cleanup
  soap_destroy(soap);
  soap_end(soap);
  soap_free(soap);
}

int main()
{
  // create a context with strict XML validation and exclusive XML canonicalization for WS-Security enabled
  struct soap *soap = soap_new1(/*SOAP_XML_STRICT |*/ SOAP_XML_CANONICAL);
  soap->connect_timeout = soap->recv_timeout = soap->send_timeout = 10; // 10 sec

  // create the proxies to access the ONVIF service API at HOSTNAME
  //DeviceBindingProxy proxyDevice(soap);
  DeviceService device(soap);
  device.set_credentials(HOSTNAME, USERNAME, PASSWORD);
  MediaService media(soap);
  MediaBindingProxy proxyMedia(soap);

  // get device info and print
  _tds__GetDeviceInformationResponse dev_info = device.device_info();
  std::cout << "Manufacturer:    " << dev_info.Manufacturer << std::endl;
  std::cout << "Model:           " << dev_info.Model << std::endl;
  std::cout << "FirmwareVersion: " << dev_info.FirmwareVersion << std::endl;
  std::cout << "SerialNumber:    " << dev_info.SerialNumber << std::endl;
  std::cout << "HardwareId:      " << dev_info.HardwareId << std::endl;

  // get device capabilities and print media
  _tds__GetCapabilitiesResponse capabilities = device.capabilities();

  std::cout << "XAddr:        " << capabilities.Capabilities->Media->XAddr << std::endl;
  if (capabilities.Capabilities->Media->StreamingCapabilities)
  {
    if (capabilities.Capabilities->Media->StreamingCapabilities->RTPMulticast)
      std::cout << "RTPMulticast: " << (*capabilities.Capabilities->Media->StreamingCapabilities->RTPMulticast ? "yes" : "no") << std::endl;
    if (capabilities.Capabilities->Media->StreamingCapabilities->RTP_USCORETCP)
      std::cout << "RTP_TCP:      " << (*capabilities.Capabilities->Media->StreamingCapabilities->RTP_USCORETCP ? "yes" : "no") << std::endl;
    if (capabilities.Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP)
      std::cout << "RTP_RTSP_TCP: " << (*capabilities.Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP ? "yes" : "no") << std::endl;
  }

  // set the Media proxy endpoint to XAddr
  media.set_media_endpoint(capabilities);
  //proxyMedia.soap_endpoint = capabilities.Capabilities->Media->XAddr.c_str();

  // get device profiles
  //_trt__GetProfiles GetProfiles;
  _trt__GetProfilesResponse profiles = media.profiles();
  /*set_credentials(soap);
  if (proxyMedia.GetProfiles(&GetProfiles, GetProfilesResponse))
    report_error(soap);
  check_response(soap);*/

  // for each profile get snapshot
    std::cout << profiles.Profiles.size() << std::endl;
  for (int i = 0; i < profiles.Profiles.size(); ++i)
  {
    // get snapshot URI for profile
    _trt__GetSnapshotUri GetSnapshotUri;
    _trt__GetSnapshotUriResponse GetSnapshotUriResponse;
    GetSnapshotUri.ProfileToken = profiles.Profiles[i]->token;
    set_credentials(soap);
    if (media.media().GetSnapshotUri(&GetSnapshotUri, GetSnapshotUriResponse))
      report_error(soap);
    check_response(soap);
    std::cout << "Profile name: " << profiles.Profiles[i]->Name << std::endl;
  }

  // free all deserialized and managed data, we can still reuse the context and proxies after this
  soap_destroy(soap);
  soap_end(soap);

  // free the shared context, proxy classes must terminate as well after this
  soap_free(soap);


  return 0;
}

/******************************************************************************\
 *
 *	WS-Discovery event handlers must be defined, even when not used
 *
\******************************************************************************/

void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{ }

void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{ }

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *ProbeMatches)
{
  return SOAP_WSDD_ADHOC;
}

void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *ProbeMatches)
{ }

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
  return SOAP_WSDD_ADHOC;
}

void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char * SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{ }

int SOAP_ENV__Fault(struct soap *soap, char *faultcode, char *faultstring, char *faultactor, struct SOAP_ENV__Detail *detail, struct SOAP_ENV__Code *SOAP_ENV__Code, struct SOAP_ENV__Reason *SOAP_ENV__Reason, char *SOAP_ENV__Node, char *SOAP_ENV__Role, struct SOAP_ENV__Detail *SOAP_ENV__Detail)
{
  // populate the fault struct from the operation arguments to print it
  soap_fault(soap);
  // SOAP 1.1
  soap->fault->faultcode = faultcode;
  soap->fault->faultstring = faultstring;
  soap->fault->faultactor = faultactor;
  soap->fault->detail = detail;
  // SOAP 1.2
  soap->fault->SOAP_ENV__Code = SOAP_ENV__Code;
  soap->fault->SOAP_ENV__Reason = SOAP_ENV__Reason;
  soap->fault->SOAP_ENV__Node = SOAP_ENV__Node;
  soap->fault->SOAP_ENV__Role = SOAP_ENV__Role;
  soap->fault->SOAP_ENV__Detail = SOAP_ENV__Detail;
  // set error
  soap->error = SOAP_FAULT;
  // handle or display the fault here with soap_stream_fault(soap, std::cerr);
  // return HTTP 202 Accepted
  return soap_send_empty_response(soap, SOAP_OK);
}

/******************************************************************************\
 *
 *	OpenSSL
 *
\******************************************************************************/

#ifdef WITH_OPENSSL

struct CRYPTO_dynlock_value
{ MUTEX_TYPE mutex;
};

static MUTEX_TYPE *mutex_buf;

static struct CRYPTO_dynlock_value *dyn_create_function(const char *file, int line)
{ struct CRYPTO_dynlock_value *value;
  value = (struct CRYPTO_dynlock_value*)malloc(sizeof(struct CRYPTO_dynlock_value));
  if (value)
    MUTEX_SETUP(value->mutex);
  return value;
}

static void dyn_lock_function(int mode, struct CRYPTO_dynlock_value *l, const char *file, int line)
{ if (mode & CRYPTO_LOCK)
    MUTEX_LOCK(l->mutex);
  else
    MUTEX_UNLOCK(l->mutex);
}

static void dyn_destroy_function(struct CRYPTO_dynlock_value *l, const char *file, int line)
{ MUTEX_CLEANUP(l->mutex);
  free(l);
}

void locking_function(int mode, int n, const char *file, int line)
{ if (mode & CRYPTO_LOCK)
    MUTEX_LOCK(mutex_buf[n]);
  else
    MUTEX_UNLOCK(mutex_buf[n]);
}

unsigned long id_function()
{ return (unsigned long)THREAD_ID;
}

int CRYPTO_thread_setup()
{ int i;
  mutex_buf = (MUTEX_TYPE*)malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
  if (!mutex_buf)
    return SOAP_EOM;
  for (i = 0; i < CRYPTO_num_locks(); i++)
    MUTEX_SETUP(mutex_buf[i]);
  CRYPTO_set_id_callback(id_function);
  CRYPTO_set_locking_callback(locking_function);
  CRYPTO_set_dynlock_create_callback(dyn_create_function);
  CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
  CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);
  return SOAP_OK;
}

void CRYPTO_thread_cleanup()
{ int i;
  if (!mutex_buf)
    return;
  CRYPTO_set_id_callback(NULL);
  CRYPTO_set_locking_callback(NULL);
  CRYPTO_set_dynlock_create_callback(NULL);
  CRYPTO_set_dynlock_lock_callback(NULL);
  CRYPTO_set_dynlock_destroy_callback(NULL);
  for (i = 0; i < CRYPTO_num_locks(); i++)
    MUTEX_CLEANUP(mutex_buf[i]);
  free(mutex_buf);
  mutex_buf = NULL;
}

#else

/* OpenSSL not used */

int CRYPTO_thread_setup()
{
  return SOAP_OK;
}

void CRYPTO_thread_cleanup()
{ }

#endif
