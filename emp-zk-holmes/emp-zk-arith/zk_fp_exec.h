#ifndef ZK_FP_EXECUTION_H__
#define ZK_FP_EXECUTION_H__

#include "emp-zk-holmes/emp-vole/emp-vole.h"
#include "emp-zk-holmes/emp-zk-arith/ostriple.h"

#ifdef ARGUMENT_EXPORT
typedef struct {
	bool is_input_or_witness;
	uint64_t idx;
} WireIndex;
#endif

class ZKFpExec {
public:
	int64_t gid = 0;
	__uint128_t pub_mac;
	int B, c;

	static ZKFpExec *zk_exec;

	ZKFpExec() {}
	virtual ~ZKFpExec() {}

	virtual int print_total_triple() = 0;

	virtual void feed(__uint128_t& label, const uint64_t& value) = 0;

	virtual void feed(__uint128_t *label, const uint64_t *value, int len) = 0;

	virtual void reveal(__uint128_t *label, uint64_t *value, int len) = 0;

	virtual void reveal_check(__uint128_t *label, const uint64_t *value, int len) = 0;

	virtual void reveal_check_zero(__uint128_t *label, int len) = 0;

	#ifdef ARGUMENT_EXPORT
	virtual void start_argument_exporter(const char* script_filename, const char* input_filename) = 0;

	virtual void end_argument_exporter(int num_repetitions) = 0;

	virtual WireIndex write_feed(const uint64_t &value) = 0;

	virtual WireIndex write_add_gate(const WireIndex &idx_a, const WireIndex &idx_b) = 0;

	virtual WireIndex write_mul_gate(const WireIndex &idx_a, const WireIndex &idx_b) = 0;

	virtual WireIndex write_mul_const_gate(const WireIndex &idx_a, const uint64_t &value) = 0;

	virtual WireIndex write_pub_label(const uint64_t &a) = 0;
	#endif

	virtual __uint128_t add_gate(const __uint128_t& a, const __uint128_t& b) = 0;

	virtual __uint128_t mul_gate(const __uint128_t& a, const __uint128_t& b) = 0;

	virtual __uint128_t mul_const_gate(const __uint128_t& a, const uint64_t& b) = 0;

	virtual __uint128_t pub_label(const uint64_t&a) = 0;
};

// ZKFpExec * ZKFpExec::zk_exec = nullptr;
#endif
