#ifndef ZK_FP_EXECUTIION_PROVER_H__
#define ZK_FP_EXECUTIION_PROVER_H__
#include "emp-zk-holmes/emp-zk-arith/zk_fp_exec.h"

#ifdef ARGUMENT_EXPORT
#include <iostream>
#include <fstream>
#endif

template<typename IO>
class ZKFpExecPrv : public ZKFpExec {
public:
	FpOSTriple<IO> *ostriple;
	IO* io = nullptr;
	int triple_counter = 0;

	ZKFpExecPrv(IO** ios, int threads) : ZKFpExec() {
		PRG prg(fix_key);
		prg.random_block((block*)&this->pub_mac, 1);
		this->pub_mac = mod_uint128(this->pub_mac & (__uint128_t)0xFFFFFFFFFFFFFFFFULL);
		this->io = ios[0];
		this->ostriple = new FpOSTriple<IO>(ALICE, threads, ios);
	}

	~ZKFpExecPrv() {
		delete ostriple;
	}

	int print_total_triple() {
		return triple_counter;
	}

	/* 
	 * Prover is the receiver in iterative COT
	 * interface: get 1 authenticated bit
	 * authenticated message, last bit as the value
	 * embeded in label
	 */
	void feed(__uint128_t& label, const uint64_t& val) {
		triple_counter += 1;
		label = this->ostriple->authenticated_val_input(val);
	}

	void feed(__uint128_t *label, const uint64_t *val, int len) {
		triple_counter += len;
		this->ostriple->authenticated_val_input(label, val, len);
	}

	/*
	 * check correctness of triples using cut and choose and bucketing
	 * check correctness of the output
	 */
	void reveal(__uint128_t *mac, uint64_t *value, int len) {
		this->ostriple->reveal_send(mac, value, len);
	}

	void reveal_check(__uint128_t *mac, const uint64_t *value, int len) {
		this->ostriple->reveal_check_send(mac, value, len);
	}

	void reveal_check_zero(__uint128_t *mac, int len) {
		this->ostriple->reveal_check_zero(mac, len);
	}

	__uint128_t add_gate(const __uint128_t& a, const __uint128_t& b) {
		__uint128_t val = mod_uint128((a>>64)+(b>>64));
		__uint128_t mac = mod_uint128((a&0xFFFFFFFFFFFFFFFFULL)+(b&0xFFFFFFFFFFFFFFFFULL));
		return (val<<64)^mac;
	}

	__uint128_t mul_gate(const __uint128_t& a, const __uint128_t& b) {
		++this->gid;
		triple_counter += 1;
		return ostriple->auth_compute_mul_send(a, b);
	}

	__uint128_t mul_const_gate(const __uint128_t& a, const uint64_t& b) {
		return ostriple->auth_mac_mul_const(a, (__uint128_t)b);
	}

	__uint128_t pub_label(const uint64_t& a) {
		return (__uint128_t)makeBlock(a, (uint64_t)this->pub_mac);
	}

	#ifdef ARGUMENT_EXPORT
	std::ofstream script_file_io;
	std::ofstream input_file_io;

	uint64_t next_available_inputwitness_idx;
	uint64_t next_available_intermediate_idx;
	uint64_t num_input_or_witness;

	void start_argument_exporter(const char* script_filename, const char* input_filename) {
		cout << "script filename: " << script_filename << endl;
		this->script_file_io.open(script_filename);
		cout << "input filename: " << input_filename << endl;
		this->input_file_io.open(input_filename);

		next_available_inputwitness_idx = 0;
		next_available_intermediate_idx = 0;

		num_input_or_witness = 0;

		this->script_file_io << "v = []\r\n";
		this->script_file_io << "iw = []\r\n";
		this->script_file_io << "@function_block" << "\r\n";
		this->script_file_io << "def f():" << "\r\n";
	}

	void end_argument_exporter(int num_repetitions) {
		this->script_file_io << "\r\n";
		this->script_file_io << "@function_block" << "\r\n";
		this->script_file_io << "def g():" << "\r\n";
		this->script_file_io << "\tdel v[:]\r\n";
		this->script_file_io << "\tdel iw[:]\r\n";
		this->script_file_io << "\tfor i in range(" << num_input_or_witness << "):\r\n";
		this->script_file_io << "\t\tiw.append(sint.get_private_input_from(0))\r\n";
		this->script_file_io << "\tf()\r\n";
		this->script_file_io << "\r\n";
		this->script_file_io << "for t in range(" << num_repetitions << "):\r\n";
		this->script_file_io << "\tg()\r\n";

		this->script_file_io.close();
		this->input_file_io.close();
	}

	WireIndex write_feed(const uint64_t &value) {
		WireIndex res;
		res.is_input_or_witness = true;
		res.idx = this->next_available_inputwitness_idx++;

		num_input_or_witness ++;
		this->input_file_io << value << "\n";

		return res;
	}

	WireIndex write_add_gate(const WireIndex &idx_a, const WireIndex &idx_b) {
		uint64_t res_id = this->next_available_intermediate_idx++;

		WireIndex res;
		res.is_input_or_witness = false;
		res.idx = res_id;

		this->script_file_io << "\tv.append(";

		if(idx_a.is_input_or_witness) {
			this->script_file_io << "iw[" << idx_a.idx << "]";
		}else{
			this->script_file_io << "v[" << idx_a.idx << "]";
		}

		if(idx_b.is_input_or_witness) {
			this->script_file_io << "+ iw[" << idx_b.idx << "])\r\n";
		}else{
			this->script_file_io << "+ v[" << idx_b.idx << "])\r\n";
		}

		return res;
	}

	WireIndex write_mul_gate(const WireIndex &idx_a, const WireIndex &idx_b) {
		uint64_t res_id = this->next_available_intermediate_idx++;

		WireIndex res;
		res.is_input_or_witness = false;
		res.idx = res_id;

		this->script_file_io << "\tv.append(";

		if(idx_a.is_input_or_witness) {
			this->script_file_io << "iw[" << idx_a.idx << "]";
		}else{
			this->script_file_io << "v[" << idx_a.idx << "]";
		}

		if(idx_b.is_input_or_witness) {
			this->script_file_io << "* iw[" << idx_b.idx << "])\r\n";
		}else{
			this->script_file_io << "* v[" << idx_b.idx << "])\r\n";
		}

		return res;
	}

	WireIndex write_mul_const_gate(const WireIndex &idx_a, const uint64_t &value) {
		uint64_t res_id = this->next_available_intermediate_idx++;

		WireIndex res;
		res.is_input_or_witness = false;
		res.idx = res_id;

		this->script_file_io << "\tv.append(";

		if(idx_a.is_input_or_witness) {
			this->script_file_io << "iw[" << idx_a.idx << "]";
		}else{
			this->script_file_io << "v[" << idx_a.idx << "]";
		}

		this->script_file_io << "* cint("<< value << "))\r\n";

		return res;
	}

	WireIndex write_pub_label(const uint64_t &value) {
		uint64_t res_id = this->next_available_intermediate_idx++;

		WireIndex res;
		res.is_input_or_witness = false;
		res.idx = res_id;

		this->script_file_io << "\tv.append(cint(" << value << "))\r\n";

		return res;
	}
	#endif
};
#endif
