
#ifndef AF_TRANSACTION_H
#define AF_TRANSACTION_H

#include "common.h"
#include "utility.h"
#include "aframe/shared_types.h"
#include "aframe/binary_IO.h"
#include "curv/script_types.h"



// 
namespace curv {

  // 
  typedef std::map<std::string, af::bytearray> TxMap; 
  // 
  bool FetchTx (af::bytearray& out, bool mainnet, const std::string& txid_hex, TxMap& cache);

  // 
  struct TxFetcher  {

    TxMap cache; 
    //
    TxFetcher () : caache() {} 
    ~TxFetcher() {} 

    bool Fetch (af::bytearray& out, const std::string& txid_hex, bool mainnet);
   
  };

  // input transaction
  struct TxIn { 
    af::byte32   prev_txid;   // 32b LE
    unsigned int prev_index;  // int LE

    curv::command_list script_sig; 
    //af::bytearray script_sig; // variable
    unsigned int  sequence;   // 
  }; 

  typedef std::vector<TxIn> TxInputs; 

  // output transaction
  struct TxOut { // r the diffrint
    size_t        amount; 
    af::bytearray script_bin;
  }; 

  typedef std::vector<TxOut>  TxOutputs;

  // 
  // we
  struct Transaction {
    //
    Transaction () : version (0), inputs (), locktime (0), outputs (0) {
    }

    // 
    unsigned int version;    
    TxInputs       inputs;
    unsigned int locktime; //; 
    TxOutputs     outputs; 
      
  };
  
  //
  //
  void print_txin (const TxIn& txin, size_t len); 

  //
  //
  void print_txo  (const TxOut& txo, size_t len);
  
  //
  // 
  size_t ReadTransaction  (Transaction& out, af::ReadStreamRef rs);
  

  //
  // serialize 
  size_t WriteTransaction (af::WriteStreamRef ws, const Transaction& out); 
  
} // curv
#endif
