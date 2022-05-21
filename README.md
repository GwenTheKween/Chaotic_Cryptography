# Chaotic_Cryptography
DO NOT USE THIS. This is a re-do of a security research, not ready for real world deployment


### Memorabilia
This project was insipired by my time as an undergrad researcher in USP, studying under Odemir Bruno and Jeaneth Machicao. With their guidance, the concept of this system was described and a first c++ draft was created, but unfortunately never finished.

Until now.

# Introduction

This Cryptographic system is based on the i-Logistic map equation. 
Machicao published many papers on the k-logistic map[1], reaching the conclusion that it was a suitable equation for a Cryptographically Secure Pseudo Random Number Generator (CSPRNG). 

## The Logistic Map

This is a pretty famous function for chaos studies. The hard-to-explain, impossible to predict logistic map looks like so:

$$ x_{i+1} = \alpha * x_{i} * (1 - x_i) $$
$$ x \in [0,1] $$

Quite the complicated mathematical function, right?

Once $\alpha$ approaches 4, this system - despite being deterministic - start to behave unpredictably. This is what we call chaos.

That is, unpredictably until we talk about cryptography. See, the probability density for each possible value of x is not exactly flat, it is more of a sharp U, as the probability of getting a 0 or a 1 exponentially increase once we get close to rounding errors in floating point variables.

This small but noticeable probability shift makes the cryptographic security of logistic map-based cryptographies be questionable at best. This is where Machicao's research come in:

## The K-Logistic map

Machicao[2] sought to fix the problems with using the logistic map by discarding the first k digits of $x$ whenever we decide to sample it. This doesn't change the logistic map equation itself, but if before we just used $x_i$ as our answer, we now use:

$$ PRNG_i = 10^k * x_i mod 1 $$

Recent studies[3] show that this is reliable enough to be considered cryptographically secure, and - more importantly to me - have given a good way to calculate when something can be considered reliable enough to be secure, so I can come back to my undergrad plans

## The i-Logistic map

As someone with a mind for computers and binaries, I spoke to Machicao about approaching this problem in a more computer-friendly way. Instead of discarding the first k digits of the number in decimal notation, we could discard the first k __binary digits__ (bits), making everything faster to compute and more suited to low power computers$^*$.

$^*$ Sort of. This only really happens if we are using fixed point notation for our internal variables, as floating point notation isn't subject to such great hacks. Since it can also show problems with rounding errors (which separate 2 equal states exponentially fast) and it is generally slower than fixed point math even without hacking bits, fFixed point notation was chosen for the project.

Anyway, I needed a way to differentiate my idea from Machicao's, and since hers used k as the exponent and I was going for bits, i seemed the best choice.

# The Cryptographic system

With all that preamble done, let's look at what I am actually doing here:

I want to design a simple stream cipher based on the i-logistic map, use a simple reversible operation to mix it with the plaintext (haven't decided yet on XOR, modular addition, or both) and spit out the text.

For option 1 we have:

$$ x_{i+1} = \alpha * x_i * (1-x_i) $$

$$ r_i = 2^i * x_i mod 1 $$

$$ C_i = r_i \oplus P_i $$

For the second option we have:

$$ x_{i+1} = \alpha * x_i * (1-x_i) $$

$$ r_i = 2^i * x_i mod 1 $$

$$ C_i = (r_i + P_i) \% 256 $$

Where:

* $ r_i $ is the random number generated in the i-th iteration;
* $ P_i $ is the i-th plaintext character;
* $ C_i $ is the i-th ciphertext character;
* $ \oplus $ is the XOR operation;
* $ % $ is the mod operation;

For decrypting, everything works the same, with the exception of the second option, where we use modular subtraction instead.

## Key Generation details

Since chaotic systems are very sensitive to change, every last bit of the state is useful for us when calculating keyspace. Assuming we are using n bytes for the state, the minimum amount of bits of keyspace - assuming a suitable key generation algorithm - would be 8*n.

Seeing as we need at least 256 bits to have something considered secure against brute forcing for simetric encryption, we need to use at least 32 bytes.

There is also a possibility of parameterizing $\alpha$ based on the key, but as we want a number close to 4 for good chaotic properties, we can't assume all n bytes can be used, so a conservative estimate would assume half of those can be freely changed, landing us at 12*n bits (8*n for the state, 4*n for $\alpha$), requiring only 22 bytes.

A conveniently suitable keygen algorithm - based on user-inputted data - would use a secure hashing algorithm that results in a hash with at least as many bits as the key we are using, so for 256 bits, we can use SHA256, and for 512 bits we can use SHA512. The second one will be assumed for the rest of this explanation.

With those demonstrably secure random bits generated, our key space is limited to 512 or $\frac{3}{2}$ times the amount of bits our internal state has.

### The wrench in the beautiful system

Well, remember when I said we need fixed point notation, of arbitrary precision no less - seeing how many bytes we need for a system secure against brute force?

This means I have to make my own system - as GMP, despite technicaly covering it, was too general to give the speed I wanted.

The simple system I created (in fixed.h) is beautiful, but has a fatal flaw: It needs a power of 2 amount of bytes. This means we can't skirt the edge of acceptable to get the most performance out of our system, but we can use 32 bytes for state.

### Final keyspace

The final keyspace that can be reliably generated based on the given limitations is 32 bytes for internal state and 16 bytes for the $\alpha$ parameter, for a total of 384 bits. Not too shabby, but far from perfect. Bumping the state to 64, however, hits the 512 bits brick wall, so we'll stick to 32 bytes for now.

# Results

There are no results for now, but as I develop and implement things, I'll keep this readme posted

# Future work

After I finish the basic version, there are other ways to use the i-logistic map's ideas.

1. Use the tent map, instead of the logistic map, as the source of randomness: It is a coarser, but easier to compute by hand, equation that can present similar results to the logistic map. It's just a bit of variety, doubling the keyspace of the system, i guess.
2. Modularize the system so my cryptography can take any PRNG I create and just run with it, assuming it will work. It's gonna make every other item in this list much easier
3. Use a combined logistic-map and tent-map[4]; This is not my original idea, but it is a good one nonetheless. The new system will use 2 possible compositions of logistic and tent maps, instead of only using one or the other. The paper says it is more suitable to cryptagraphic purposes, but I expect a big drop in performance. This will increase the amount of PRNGs by 2 again, for a total of 4.
4. Use one map to add perturbations to another. This idea is my own (With Odemir's nudge back when I researched with him). The idea is that there will be 2 maps running, one for samppling as the CSPRNG, the other perturbing the first after a random amount of iterations. The idea is that the perturbations are so infrequent that there will not be enough information to fully describe the internal system of the CSPRNG at any point. We could also mix-n-match the previously described systems, instead of just using a single map, for a grand total of 16 combinations + 4 non-perturbed systems, 20 CSPRNGs (though using the same one twice might be bad, so possibly only 16).
5. Use a secondary map for block cipher-ing this system. Right now, this is a strea cipher, which is faster but less secure than block ciphers. We need a secondary source of random-ness to create "block cipher modes of operation" and really blow this home-project out of proportion. Since we can mix-n-match again, we could have as many as 400 (maybe only 256) options just for CSPRNG alone (though choosing the same twice could also be a bad idea, so we might only have 240).

# Citations
[1] https://aip.scitation.org/doi/abs/10.1063/1.4983836

[2] https://arxiv.org/pdf/2111.05101.pdf

[3] https://arxiv.org/pdf/2001.03549.pdf

[4] https://arxiv.org/abs/2111.05101
