
#include "network.h"
#include "aframe/hash.h"


using namespace af;
using namespace ffm; 
// ---------------------------------------------------------------------
//
// ---------------------------------------------------------------------
uint64 bmx::Network::Envelope::Read (Struc& envel, ReadStreamRef rs, bool mainnet) {

  FN_SCOPE ();

  uint64 readlen = 0;

  uint32 readmagic = 0;
  readlen += rs->Read (&readmagic, sizeof(uint32));

  // std::string b4str, aftstr;
  swap_endian<uint32> (&readmagic);
  
  if (mainnet) { 
    assert (readmagic == Network::kMAINNET_MAGIC);
  }
  else {
    assert (readmagic == Network::kTESTNET_MAGIC); 
  }

  envel.magic = readmagic; 
  
  envel.command.resize (12, byte(0));  
  readlen += rs->Read (&envel.command[0], 12); 
  while (envel.command.back () == byte{0x0}) envel.command.pop_back ();

  {
    std::string chars;
    for (auto e : envel.command)
      chars += std::to_integer<uint8>(e);
    printf("   command size(%zu) [%s]\n", envel.command.size (), chars.c_str());
  }

  //printf ( "    enve command size[%zu]\n",   envel.command.size ()); 
  
  uint32 payloadlen = 0;
  readlen += rs->Read (&payloadlen, sizeof(uint32));
  printf ("    payload length [%u] \n", payloadlen);

  uint32 rdchecksum = 0; 
  readlen += rs->Read (&rdchecksum, sizeof (uint32)); 

  envel.payload.resize (payloadlen, byte{0});
  readlen += rs->Read(&envel.payload[0], payloadlen); 

  union {
    uint32 checksum;
    byte bytes_cs_[4]; 
  };
  
  digest32 dig ;
  af::hash256 (dig, &envel.payload[0], payloadlen);
  std::copy (dig.begin (), dig.begin() + 4, bytes_cs_);

  if (rdchecksum != checksum)  {
    printf ("    [%s] CHECKSUM DOES NOT MATCH !!  \n", __FUNCTION__); 
    }
  assert (rdchecksum == checksum);

  return readlen; 
}

// ---------------------------------------------------------------------
//
// ---------------------------------------------------------------------
uint64 bmx::Network::Envelope::Write (WriteStreamRef ws, const Struc& nv) {
  uint64 writelen = 0;

  
  // magic
  uint32  magicbytes = nv.magic; 
  swap_endian<uint32>(&magicbytes); // comment about LE
  writelen += ws->Write (&magicbytes, sizeof(uint32)); 
  
  // command
  bytearray cmdpad = nv.command;
  while (cmdpad.size () < 12) cmdpad.push_back (byte (0)); 
  writelen += ws->Write (&cmdpad[0], 12); 
  
  // payload length
  uint32 payloadlen = nv.payload.size (); 
  writelen +=  ws->Write (&payloadlen, sizeof (uint32)); 
  
  // checksum bytes
  digest32 thegest; 
  writelen += ws->Write (&af::hash256(thegest, &nv.payload[0], nv.payload.size ())[0], 4);
  
  // payload 
  writelen += ws->Write (&nv.payload[0], nv.payload.size ()); 
  
  return writelen; 
}


// ---------------------------------------------------------------------
//
// ---------------------------------------------------------------------
int bmx::Network::Envelope::Send (af::conn_ref conn, const bmx::Network::Envelope::Struc& ne, int flags) {
  FN_SCOPE (); 

  const int32 bufsize = 1024; 
  std::array<byte, bufsize> sendbuf;

  uint64    writelen_send = 0;

  //WriteStreamRef ws_ne = CreateWriteMemStream (&sendbuf[0], 1024); 
  writelen_send += Network::Envelope::Write (CreateWriteMemStream (&sendbuf, bufsize), ne);
  printf ("    serialized ne len [%zu] \n", writelen_send); 

  return conn->Send (&sendbuf, writelen_send, flags);  
  }
// ---------------------------------------------------------------------
//
// ---------------------------------------------------------------------
int bmx::Network::Envelope::Recv (MessageCB* const cb, af::conn_ref conn, bool mainnet, int flags) {
  FN_SCOPE (); 
  
  const int32 bufsize = 1024; 
  bytearray   buf     (bufsize, byte(0));

  int recvlen = 0; 
  
  recvlen += conn->Recv (&buf[recvlen], bufsize - recvlen, flags);

  //
  network_envelope ne; 
  ReadStreamRef rs = CreateReadMemStream (&buf[0], recvlen);

  uint64 readlen = 0;
  uint32 readmagic = 0;
  readlen += rs->Read (&readmagic, sizeof(uint32));

  // std::string b4str, aftstr;
  swap_endian<uint32> (&readmagic);
  
  if (mainnet) { 
    assert (readmagic == Network::kMAINNET_MAGIC);
  }
  else {
    assert (readmagic == Network::kTESTNET_MAGIC); 
  }

  ne.magic = readmagic; 
  
  ne.command.resize (12, byte(0));  
  readlen += rs->Read (&ne.command[0], 12); 
  while (ne.command.back () == byte{0x0}) ne.command.pop_back ();

  {
    std::string chars;
    for (auto e : ne.command)
      chars += std::to_integer<uint8>(e);
    printf("   command size(%zu) [%s]\n", ne.command.size (), chars.c_str());
  }

  //printf ( "    enve command size[%zu]\n",   envel.command.size ()); 
  uint32 payloadlen = 0;
  readlen += rs->Read (&payloadlen, sizeof(uint32));
  printf ("    payload length [%u] \n", payloadlen);

  uint32 rdchecksum = 0; 
  readlen += rs->Read (&rdchecksum, sizeof (uint32));

  //
  int32 total_msg_length = readlen + payloadlen; 
  while (recvlen < total_msg_length) { 
    recvlen += conn->Recv (&buf[recvlen], bufsize - recvlen, flags);
  }

  ne.payload.resize (payloadlen, byte{0});
  readlen += rs->Read(&ne.payload[0], payloadlen); 

  union {
    uint32 checksum;
    byte bytes_cs_[4]; 
  };
  
  digest32 dig ;
  af::hash256 (dig, &ne.payload[0], payloadlen);
  std::copy (dig.begin (), dig.begin() + 4, bytes_cs_);

  if (rdchecksum != checksum)  {
    printf ("    [%s] CHECKSUM DOES NOT MATCH !!  \n", __FUNCTION__); 
    }
  assert (rdchecksum == checksum);

  //
  if (payloadlen && cb) {

    std::string cmd_s;
    for (auto e : ne.command)
      cmd_s += std::to_integer<uint8>(e);
    printf("   command size(%zu) [%s]\n", cmd_s.size(),  cmd_s.c_str());

    ReadStreamRef rspayload = CreateReadMemStream (&ne.payload[0], payloadlen) ;
    if (cmd_s == "version") {
      Network::Message::Version vers;
      int readlen = Network::Message::Read (vers, rspayload, mainnet); 
      cb->Do (vers, ne, mainnet); 
    }
    else if (cmd_s == "verack") {
      //Network::Message::VerAck verack; 
      cb->Do (Network::Message::VerAck(), ne, mainnet);
    }
    // else if (ne.command == "" ) {
    //   cb->Do (); 
    // }
       
  }

  return recvlen; 
}
  

// ---------------------------------------------------------------------
//
// ---------------------------------------------------------------------
std::string& bmx::Network::Envelope::Format (std::string& str, const bmx::Network::Envelope::Struc& enve) {
  FN_SCOPE (); 
  std::string s0, s1, s2;

  printf ( "    fmt command size[%zu]\n",   enve.command.size ()); 

  str = std::string ("Envelope {\n")
  + "  command {" + hex::encode (s0, &enve.command[0], enve.command.size()) + "}\n"
  + "  payload {" + hex::encode (s1, &enve.payload[0], enve.payload.size()) + "}\n"
  + "  magic {" +   hex::encode (s2, &enve.magic     , sizeof (uint32))     + "}\n"
  +  "}\n";

  return str; 
}

// ---------------------------------------------------------------------
//
// ---------------------------------------------------------------------
bmx::Network::Envelope::Struc& bmx::Network::Envelope::Payload (
   bmx::Network::Envelope::Struc&        oenve,
   const bmx::Network::Message::Version& vers,
   bool                                  mainnet)
{ 
  FN_SCOPE ();

  const std::string version_s             = "version";

  //SizeOf (const bmx::Network::Message::Version&  vers); 
  size_t sizeOf_MessageVersion = bmx::Network::Message::SizeOf (vers); 

  printf ( "    sizeOf_MessageVersion [%zu]\n", sizeOf_MessageVersion); 
  
  //
  oenve.magic = mainnet ? Network::kMAINNET_MAGIC : Network::kTESTNET_MAGIC; 
  puts ("   >> 104\n"); 
  // command
  oenve.command.clear (); 
  to_bytes (oenve.command, version_s);


  // {
  //   printf ("   command size [%zu]\n",  oenve.command.size ());
  //   std::string chars;
  //   for (auto e : oenve.command)
  //     chars += std::to_integer<uint8>(e);
  //   printf("   chars[%s]\n", chars.c_str());
  // }

  oenve.payload.resize(sizeOf_MessageVersion); 
  auto writelen_vers = bmx::Network::Message::Write (CreateWriteMemStream (&oenve.payload[0], sizeOf_MessageVersion), vers);
  assert (writelen_vers == sizeOf_MessageVersion); 

  return oenve; 
}

// ---------------------------------------------------------------------
//
// ---------------------------------------------------------------------
bmx::Network::Envelope::Struc& bmx::Network::Envelope::Payload (
    bmx::Network::Envelope::Struc& oenve,
    const Message::VerAck&         msg,
    bool                           mainnet)
{

  FN_SCOPE ();

  const std::string version_s             = "version";
  const auto        sizeOf_MessageVersion = sizeof(Message::Version);



  return oenve;
}


bmx::Network::Envelope::Struc& bmx::Network::Envelope::Payload (
    bmx::Network::Envelope::Struc& oenve,
    const bmx::Network::Message::GetHeaders& msg,
    bool mainnet) {
  return oenve;
}

bmx::Network::Envelope::Struc& bmx::Network::Envelope::Payload (bmx::Network::Envelope::Struc& oenve, const bmx::Network::Message::Headers& msg, bool mainnet) {
  return oenve;
}

bmx::Network::Envelope::Struc& bmx::Network::Envelope::Payload (bmx::Network::Envelope::Struc& oenve, const bmx::Network::Message::Ping& msg, bool mainnet) {
  return oenve;
}

bmx::Network::Envelope::Struc& bmx::Network::Envelope::Payload (bmx::Network::Envelope::Struc& oenve, const bmx::Network::Message::Pong& msg, bool mainnet) {
  return oenve;
}

