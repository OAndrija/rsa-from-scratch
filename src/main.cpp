#include <iostream>
#include <fstream>
#include <chrono>

#include "core/LCG.h"
#include "core/MathUtils.h"
#include "core/Primes.h"
#include "core/RSA.h"
#include "benchmarks/Benchmark.h"

void print_menu() {
    std::cout << "\t\tGENERATOR PRASTEVIL" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "1. Generiranje prastevil - naivna metoda" << std::endl;
    std::cout << "2. Generiranje prastevil - Miller-Rabin" << std::endl;
    std::cout << "3. Test prastevilnosti - naivna metoda" << std::endl;
    std::cout << "4. Test prastevilnosti - Miller-Rabin" << std::endl;
    std::cout << "5. Test LCG generatorja" << std::endl;
    std::cout << "6. Test casovne zahtevnosti generiranja prastevil" << std::endl;
    std::cout << "7. Test casovne zahtevnosti glede na parameter s" << std::endl;
    std::cout << "8. Generiranje RSA kljucev" << std::endl;
    std::cout << "9. Sifriranje stevila" << std::endl;
    std::cout << "10. Desifriranje stevila" << std::endl;
    std::cout << "11. Sifriranje datotek" << std::endl;
    std::cout << "12. Desifriranje datotek" << std::endl;
    std::cout << "13. Test casovne zahtevnosti generiranja kljucev" << std::endl;
    std::cout << "14. Test casovne zahtevnosti sifriranja in desifriranja" << std::endl;
    std::cout << "0. Izhod" << std::endl;
    std::cout << "\nIzbira: ";
}

int main() {
    LCG lcg;
    int choice;

    do {
        print_menu();
        std::cin >> choice;

        switch (choice) {
            case 1: {
                int bits;

                std::cout << "Vnesite stevilo bitov (2-32): ";
                std::cin >> bits;

                if (bits < 2 || bits > 32) {
                    std::cout << "Stevilo bitov omejeno med 2 in 32.\n" << std::endl;
                    break;
                }

                unsigned long long min = min_from_bits(bits);
                unsigned long long max = max_from_bits(bits);

                std::cout << "\nObmocje stevil: " << min << " - " << max << std::endl;

                unsigned long long prime = naive_generate(lcg, min, max);

                std::cout << "Najdeno prastevilo: " << prime << "\n" << std::endl;

                break;
            }

            case 2: {
                int bits;
                unsigned long long s;

                std::cout << "Vnesite stevilo bitov (2-32): ";
                std::cin >> bits;

                if (bits < 2 || bits > 32) {
                    std::cout << "Stevilo bitov omejeno med 2 in 32.\n" << std::endl;
                    break;
                }

                std::cout << "Vnesite parameter s: ";
                std::cin >> s;

                if (s < 1 ) {
                    std::cout << "Parameter s mora biti vsaj 1.\n" << std::endl;
                    break;
                }

                unsigned long long min = min_from_bits(bits);
                unsigned long long max = max_from_bits(bits);

                std::cout << "\nObmocje stevil: " << min << " - " << max << std::endl;

                unsigned long long prime = miller_rabin_generate(lcg, min, max, s);

                std::cout << "Najdeno (verjetno) prastevilo: " << prime << "\n" << std::endl;

                break;
            }


            case 3: {
                unsigned long long number;

                std::cout << "Vnesite stevilo (do 32 bitov): ";
                std::cin >> number;

                if (number > max_from_bits(32)) {
                    std::cout << "Stevilo je vecje od 32 bitov.\n" << std::endl;
                    break;
                }

                if (naive_test(number)) {
                    std::cout << number << " je PRASTEVILO.\n" << std::endl;
                } else {
                    std::cout << number << " je SESTAVLJENO STEVILO.\n" << std::endl;
                }

                break;
            }

            case 4: {
                unsigned long long number;
                unsigned long long s;

                std::cout << "Vnesite stevilo (do 32 bitov): ";
                std::cin >> number;

                if (number > max_from_bits(32)) {
                    std::cout << "Stevilo je vecje od 32 bitov.\n" << std::endl;
                    break;
                }

                std::cout << "Vnesite parameter s: ";
                std::cin >> s;

                if (s < 1) {
                    std::cout << "Parameter s mora biti vsaj 1.\n" << std::endl;
                    break;
                }

                if (number < 2) {   //0 and 1 are not prime
                    std::cout << number << " je SESTAVLJENO STEVILO.\n" << std::endl;
                    break;
                }

                if (miller_rabin_test(lcg, number, s)) {
                    std::cout << number << " je VERJETNO PRASTEVILO.\n" << std::endl;
                } else {
                    std::cout << number << " je SESTAVLJENO STEVILO.\n" << std::endl;
                }

                break;
            }


            case 5: {
                lcg_test(lcg);
                std::cout << std::endl;
                break;
            }

            case 6: {
                prime_generation_timing_test(lcg);
                break;
            }

            case 7: {
                miller_rabin_s_timing_test(lcg);
                std::cout << std::endl;
                break;
            }

            case 8: {
                int bits, method;

                std::cout << "Vnesite stevilo bitov(3-15): ";
                std::cin >> bits;

                if (bits < 3 || bits > 15) {
                    std::cout << "Obmocje bitov od 3 do 15.\n" << std::endl;
                    break;
                }

                std::cout << "Metoda (1 = naiva, 2 = Miller-Rabin: ";
                std::cin >> method;

                if (method != 1 && method != 2) {
                    std::cout << "Napacna izbira metode.\n" << std::endl;
                    break;
                }

                bool useMillerRabin = (method == 2);

                RSAKeys keys = generate_keys(lcg, bits, useMillerRabin, 10);

                std::cout << "Javni kljuc: " << keys.e << ", " << keys.n << std::endl;
                std::cout << "Privatni kljuc: " << keys.d << ", " << keys.n << std::endl;

                if (save_keys(keys)) {
                    std::cout << "Kljuci vspesno shranjeni v pubkey.txt in privkey.txt.\n" << std::endl;
                } else {
                    std::cout << "Napaka pri shranjevanju datotek.\n" << std::endl;
                }

                break;
            }

            case 9: {
                long long e, n , M;

                if (!load_key("pubkey.txt", e, n)) {
                    std::cout << "Najprej generirajte kljuce.\n" << std::endl;
                    break;
                }

                std::cout << "Vnesite sporocilo M: ";
                std::cin >> M;

                if (M < 0 || M >= n) {
                    std::cout << "M mora biti manjse od n.\n" << std::endl;
                    break;
                }

                long long C = modular_exponentiation(M, e, n);

                std::cout << "C = " << C << std::endl << std::endl;

                break;
            }

            case 10: {
                long long d, n, C;

                if (!load_key("privkey.txt", d, n)) {
                    std::cout << "Najprej generirajte kljuc.\n" << std::endl;
                    break;
                }

                std::cout << "Vnesite sifrirano sporocilo C: ";
                std::cin >> C;

                if (C < 0 || C >= n) {
                    std::cout << "C mora biti manjse od n.\n" << std::endl;
                    break;
                }

                long long M = modular_exponentiation(C, d, n);

                std::cout << "M = " << M << std::endl << std::endl;

                break;
            }

            case 11: {
                long long e, n;
                std::string inputName;

                if (!load_key("pubkey.txt", e, n)) {
                    std::cout << "Najprej generirajte kljuc.\n" << std::endl;
                    break;
                }

                std::cout << "Vnesite ime datoteke za sifriranje: ";
                std::cin >> inputName;

                if (encrypt_file(inputName, "enc.bin", e, n)) {
                    std::cout << "Datoteka sifrirana v enc.bin.\n" << std::endl;
                } else {
                    std::cout << "Napaka pri odpiranju datotek.\n" << std::endl;
                }

                break;
            }

            case 12: {
                long long d, n;

                if (!load_key("privkey.txt", d, n)) {
                    std::cout << "Najprej generirajte kljuc.\n" << std::endl;
                    break;
                }

                if (decrypt_file("enc.bin", "msg.bin", d, n)) {
                    std::cout << "Datoteka enc.bin desifrirana v msg.bin.\n" << std::endl;
                } else {
                    std::cout << "Napaka pri odpiranju datoteka.\n" << std::endl;
                }

                break;
            }

            case 13: {
                keygen_timing_test(lcg);
                std::cout << std::endl;
                break;
            }

            case 14: {
                crypt_timing_test(lcg);
                std::cout << std::endl;
                break;
            }

            case 0:
                break;

            default:
                std::cout << "\nNapacna izbira. Opcija " << choice << " ne obstaja." << std::endl;
                std::cout << "Poskusite ponovo.\n";
        }
    } while (choice != 0);

    return 0;
}
