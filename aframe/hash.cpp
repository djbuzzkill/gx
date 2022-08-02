#include "hash.h"


#include "gcrypt.h"


//
namespace af
{

  digest32& sha256 (digest32& out, const void* in, size_t lin)
  {
 
    //Function: void gcry_md_hash_buffer (int algo, void *digest, const void *buffer, size_t length);
    //gcry_md_hash_buffer is a shortcut function to calculate a message digest of a buffer. This function does not require a context and immediately returns the message digest of the length bytes at buffer. digest must be allocated by the caller, large enough to hold the message digest yielded by the specified algorithm algo. This required size may be obtained by using the function gcry_md_get_algo_dlen.

  //Note that in contrast to gcry_md_hash_buffers this function will abort the process if an unavailable algorithm is used.


  // gcry_error_t errval =  gcry_md_open (std::data(msg), 0);   
  //auto hay = gcry_md_hash_buffer; 
  //SHA256Init (&sha_con);
  
 
    //gcry_md_hd_t crycon; 
    //auto hnd = gcry_md_open (&crycon, GCRY_MD_SHA256 , 0); 
    gcry_md_hash_buffer (GCRY_MD_SHA256, std::data(out), in, lin);
    
    
    return out;  
  }



  digest32& hash256 (digest32& out, const void* in, size_t lin)
  {
    digest32 rnd1;

    sha256 (rnd1, in, lin);
    sha256 (out, std::data (rnd1), rnd1.size ()); 
    return out; 

  }


  digest20& ripemd160 (digest20& out, const void* in, size_t lin)
  {
    return out; 
  }
  digest20& hash160 (digest20& out, const void* in, size_t lin) {
    return out; 
  }
     


}

