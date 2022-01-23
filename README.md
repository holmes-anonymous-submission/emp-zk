# A fork of EMP-ZK

This is a fork of the [EMP-ZK](https://github.com/emp-toolkit/emp-zk) repository. 

Please attribute credits to the original authors and cite their paper**s** when you use this repository.

## Changes

The majority of changes are:

- The library is renamed as `emp-zk-holmes` to avoid conflicts.
- The function `mod(a, b)` is replaced by `mod_uint128(a)` assuming b is the hardcoded prime.
- As shown in `emp-vole/utility.h`, the prime is replaced with 2^{62}-2^{16}+1, which has some 2-arity. 
- As shown in `emp-vole/lpn.h`, the modular addition schedule of the LPN map computation has been changed to adapt to this new prime.
- Add an argument exporter to facilitate benchmarking on SPDZ.
- Add a triple counter to facilitate counting the number of triples being used.
- Remove the edabits implementation because it does not seem to work for the new prime.

## License

Since the original repository is under the MIT license, this repository is also under the MIT license.

## Regulatory issue

This repository is not subject to the U.S. Export Administration Regulation (EAR) because it is publicly available; notifications to U.S. Bureau of Industry and Security (BIS) and National Security Agency (NSA) have been sent, as the authors speculate that this repository may contain cryptographic protocols that have not been published before.

For more information about this regulatory issue, see [this post](https://www.eff.org/deeplinks/2019/08/us-export-controls-and-published-encryption-source-code-explained) by Electronic Frontier Foundation (EFF).
