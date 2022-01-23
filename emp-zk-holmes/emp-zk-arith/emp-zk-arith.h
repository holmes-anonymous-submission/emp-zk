#ifndef EMP_ZK_ARITH_H__
#define EMP_ZK_ARITH_H__
#include "emp-zk-holmes/emp-zk-arith/zk_fp_exec.h"
#include "emp-zk-holmes/emp-zk-arith/zk_fp_exec_prover.h"
#include "emp-zk-holmes/emp-zk-arith/zk_fp_exec_verifier.h"
#include "emp-zk-holmes/emp-zk-arith/triple_auth.h"
#include "emp-zk-holmes/emp-zk-arith/ostriple.h"
#include "emp-zk-holmes/emp-zk-arith/int_fp.h"
#include "emp-zk-holmes/emp-zk-arith/polynomial.h"

template<typename IO>
inline void setup_zk_arith(IO** ios, int threads, int party) {
	if(party == ALICE) {
		ZKFpExec::zk_exec = new ZKFpExecPrv<IO>(ios, threads);
		FpPolyProof<IO>::fppolyproof = new FpPolyProof<IO>(ALICE, (IO*)ios[0], ((ZKFpExecPrv<IO>*)(ZKFpExec::zk_exec))->ostriple);
	} else {
		ZKFpExec::zk_exec = new ZKFpExecVer<IO>(ios, threads);
		FpPolyProof<IO>::fppolyproof = new FpPolyProof<IO>(BOB, (IO*)ios[0], ((ZKFpExecVer<IO>*)(ZKFpExec::zk_exec))->ostriple);
	}
}

template<typename IO>
inline void finalize_zk_arith() {
	delete FpPolyProof<IO>::fppolyproof;
	delete ZKFpExec::zk_exec;
}
#endif
