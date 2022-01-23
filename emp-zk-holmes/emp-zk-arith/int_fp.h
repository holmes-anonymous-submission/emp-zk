#ifndef INT_FP_H__
#define INT_FP_H__

#include "emp-tool/emp-tool.h"
#include "emp-zk-holmes/emp-zk-bool/emp-zk-bool.h"
#include "emp-zk-holmes/emp-zk-arith/zk_fp_exec.h"
#include "emp-zk-holmes/emp-zk-arith/zk_fp_exec_prover.h"
#include "emp-zk-holmes/emp-zk-arith/zk_fp_exec_verifier.h"
#include "emp-zk-holmes/emp-zk-arith/polynomial.h"

class IntFp {
public:
	__uint128_t value;

	#ifdef ARGUMENT_EXPORT
	WireIndex intfp_idx;
	#endif

	IntFp() {}

	IntFp(IntFp *obj) {
		this->value = obj->value;
	}
	
	IntFp(uint64_t input, int party = PUBLIC) {
		if(party == PUBLIC) {
			#ifdef ARGUMENT_EXPORT
			intfp_idx = ZKFpExec::zk_exec->write_pub_label(input);
			#endif
			value = ZKFpExec::zk_exec->pub_label(input);
		} else {
			#ifdef ARGUMENT_EXPORT
			intfp_idx = ZKFpExec::zk_exec->write_feed(input);
			#endif
			ZKFpExec::zk_exec->feed(value, input);
		}
	}

	uint64_t reveal() {
		uint64_t out;
		ZKFpExec::zk_exec->reveal(&(this->value), &out, 1);
		return out;
	}

	bool reveal(uint64_t expect) {
		ZKFpExec::zk_exec->reveal_check(&(this->value), &expect, 1);
		return true;
	}

	void reveal_zero() {
		ZKFpExec::zk_exec->reveal_check_zero(&(this->value), 1);
	}	

	IntFp operator+(const IntFp& rhs) const {
		IntFp res(*this);
		#ifdef ARGUMENT_EXPORT
		res.intfp_idx = ZKFpExec::zk_exec->write_add_gate(this->intfp_idx, rhs.intfp_idx);
		#endif
		res.value = ZKFpExec::zk_exec->add_gate(this->value, rhs.value);
		return res;
	}

	IntFp operator*(const IntFp& rhs) const {
		IntFp res(*this);
		#ifdef ARGUMENT_EXPORT
		res.intfp_idx = ZKFpExec::zk_exec->write_mul_gate(this->intfp_idx, rhs.intfp_idx);
		#endif
		res.value = ZKFpExec::zk_exec->mul_gate(this->value, rhs.value);
		return res;
	}

	IntFp operator*(const uint64_t& rhs) const {
		IntFp res(*this);
		#ifdef ARGUMENT_EXPORT
		res.intfp_idx = ZKFpExec::zk_exec->write_mul_const_gate(this->intfp_idx, rhs);
		#endif
		res.value = ZKFpExec::zk_exec->mul_const_gate(this->value, rhs);
		return res;
	}

	IntFp negate() {
		IntFp res(*this);
		#ifdef ARGUMENT_EXPORT
		res.intfp_idx = ZKFpExec::zk_exec->write_mul_const_gate(this->intfp_idx, p - 1);
		#endif
		res.value = ZKFpExec::zk_exec->mul_const_gate(this->value, p-1);
		return res;
	}
};

static inline void batch_feed(IntFp *obj, uint64_t *value, int len) {
	ZKFpExec::zk_exec->feed((__uint128_t*)obj, value, len);

	#ifdef ARGUMENT_EXPORT
	for(int i = 0; i < len; i++) {
		obj[i].intfp_idx = ZKFpExec::zk_exec->write_feed(value[i]);
	}
	#endif
}

static inline void batch_reveal(IntFp *obj, uint64_t *value, int len) {
	ZKFpExec::zk_exec->reveal((__uint128_t*)obj, value, len);
}

static inline bool batch_reveal_check(IntFp *obj, uint64_t *expected, int len) {
	ZKFpExec::zk_exec->reveal_check((__uint128_t*)obj, expected, len);
	return true;
}

static inline bool batch_reveal_check_zero(IntFp *obj, int len) {
	ZKFpExec::zk_exec->reveal_check_zero((__uint128_t*)obj, len);
	return true;
}

template<typename IO>
inline void fp_zkp_poly_deg2(IntFp *x, IntFp *y, uint64_t *coeff, int len) {
	#ifdef ARGUMENT_EXPORT
	cerr << "The argument exporter does not support fp_zkp_poly_deg2." << endl;
	assert(false);
	#endif
	FpPolyProof<IO>::fppolyproof->zkp_poly_deg2((__uint128_t*)x, (__uint128_t*)y, coeff, len);
}

template<typename IO>
inline void fp_zkp_inner_prdt(IntFp *x, IntFp *y, uint64_t constant, int len) {
	#ifdef ARGUMENT_EXPORT
	cerr << "The argument exporter does not support fp_zkp_inner_prdt." << endl;
	assert(false);
	#endif
	FpPolyProof<IO>::fppolyproof->zkp_inner_prdt((__uint128_t*)x, (__uint128_t*)y, constant, len);
}
#endif
