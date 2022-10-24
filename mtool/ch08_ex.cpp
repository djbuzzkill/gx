

int test_op_checksig   (std::vector<std::string>& args) {

  FN_SCOPE();
  // def test_op_checksig(self):
  //   z = 0x7c076ff316692a3d7eb3c3bb0f8b1488cf72e1afcd929e29307032997a838a3d
  //   sec = bytes.fromhex('04887387e452b8eacc4acfde10d9aaf7f6d9a0f975aabb10d006e4da568744d06c61de6d95231cd89026e286df3b6ae4a894a3378e393e93a0f45b666329a0ae34')
  //   sig = bytes.fromhex('3045022000eff69ef2b1bd93a66ed5219add4fb51e11a840f404876325a1e8ffe0529a2c022100c7207fee197d27c618aea621406f6bf5ef6fca38681d82b2f06fddbdce6feab601')
  //   stack = [sig, sec]
  //   self.assertTrue(op_checksig(stack, z))
  //   self.assertEqual(decode_num(stack[0]), 1)


  return 0; 
}



// op_fns.cpp
int test_op_multisig (std::vector<std::string>& args);
int test_op_hash160  (std::vector<std::string>& args);


int test_p2pkh_addr(std::vector<std::string> &args) {
  FN_SCOPE();

  FFM_Env env;
  bmx::Init_secp256k1_Env (env);
  ScopeDeleter dr (env.F);

  return 0;

}


int test_p2sh_addr(std::vector<std::string> &args) {

  FN_SCOPE();

  FFM_Env env;
  bmx::Init_secp256k1_Env (env);
  ScopeDeleter dr (env.F);

  return 0;
}




//
int CH8_Ex (std::vector<std::string>& args) {

  FN_SCOPE();

  test_op_hash160  (args); 
  test_op_multisig (args); 
  test_p2pkh_addr  (args); 
  test_p2sh_addr   (args); 
  test_verify_p2sh (args); 

  return 0;
}

