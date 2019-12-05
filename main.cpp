#include "ClientDevice.h"
#include "ClientPTZ.h"
#include "ClientMedia.h"
#include <string>

#include "plugin/wsddapi.h"
#include "plugin/wsseapi.h"
#include "web_server/WebServer.h"
#include "opencv/tracker/Tracker.h"
#include "cam_ctl/CamCtl.h"

int main(int argc, char** argv){
	CamCtl cam_control("192.168.0.8", "evg", "Fh1234567890");

    WebServer server(cam_control);
    server.run();
    Tracker tracker("rtsp://Evg:Fh16121992@192.168.0.8", cam_control);
    tracker.start();
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