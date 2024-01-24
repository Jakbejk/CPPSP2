#pragma once

#include "MpInt.h"
#include <regex>
#include <array>
#include <map>
#include <optional>

/**
 * @brief Size of terminal bank - last used memory
 */
constexpr int BANK_SIZE = 5;
/**
 * @brief Regex for terminal addition input
 */
const std::regex ADDITION_REG(R"(^\s*(-?\d+|\$[0-5]{1})\s*\+\s*(-?\d+|\$[0-5]{1})\s*$)");
/**
 * @brief Regex for terminal subtraction input
 */
const std::regex SUBTRACT_REG(R"(^\s*(-?\d+|\$[0-5]{1})\s*\-\s*(-?\d+|\$[0-5]{1})\s*$)");
/**
 * @brief Regex for terminal multiply input
 */
const std::regex MULTIPLY_REG(R"(^\s*(-?\d+|\$[0-5]{1})\s*\*\s*(-?\d+|\$[0-5]{1})\s*$)");
/**
 * @brief Regex for terminal division input
 */
const std::regex DIVIDE_REG(R"(^\s*(-?\d+|\$[0-5]{1})\s*\/\s*(-?\d+|\$[0-5]{1})\s*$)");
/**
 * @brief Regex for terminal factorial input
 */
const std::regex FACTORIAL_REG(R"(^\s*(-?\d+|\$[0-5]{1})!\s*$)");

/**
 * @brief Class representing terminal for unlimited number operations.
 * @tparam bytePrecision Maximal precision of number in bytes (0 is unlimited).
 */
template<std::size_t bytePrecision> requires SizeLimitation<bytePrecision>
class MpTerm {
private :
    /**
     * @brief Inner class for rotating list - used for bank.
     */
    class RotatingVector {
    private:
        /** Maximal size of rotating list */
        const std::size_t rotatingSize;
        /** Current index of rotating list */
        std::size_t index;
        /** Items of rotating list */
        std::vector<MpInt<bytePrecision>> results;
    public:
        /**
         * @brief Set maximal size of rotating list.
         * @param size Maximal size of rotating list.
         */
        explicit RotatingVector(std::size_t size) : rotatingSize(size), index(0) {

        }

        /**
         * @brief Push item to begin of rotating list. If list is full, pop last item.
         * @param item Item to be pushed.
         */
        void push(const MpInt<bytePrecision> item) {
            if (this->index == this->rotatingSize) {
                this->results.pop_back();
            } else {
                this->index++;
            }
            this->results.insert(this->results.begin(), item);
        }

        /**
         * @return All items of rotating list.
         */
        std::vector<MpInt<bytePrecision>> getResults() const {
            return results;
        }


    };

    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ VARIABLES -------------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
private:
    /**
     * @brief Bank used for memorizing last N computed number.
     */
    RotatingVector bank;

    /**
     * @brief Map for binary operations according to operator.
     */
    std::map<char, std::function<MpInt<bytePrecision>(const MpInt<bytePrecision> &,
                                                      const MpInt<bytePrecision> &)>> binaryOperatorMap;
    /**
     * @brief Map for unary operations according to operator.
     */
    std::map<char, std::function<MpInt<bytePrecision>(const MpInt<bytePrecision> &)>> unaryOperatorMap;

    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ CONSTRUCTORS ----------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
public:
    MpTerm() : bank(BANK_SIZE), binaryOperatorMap({
                                                          {'+', [](const MpInt<bytePrecision> &a,
                                                                   const MpInt<bytePrecision> &b) { return a + b; }},
                                                          {'-', [](const MpInt<bytePrecision> &a,
                                                                   const MpInt<bytePrecision> &b) { return a - b; }},
                                                          {'*', [](const MpInt<bytePrecision> &a,
                                                                   const MpInt<bytePrecision> &b) { return a * b; }},
                                                          {'/', [](const MpInt<bytePrecision> &a,
                                                                   const MpInt<bytePrecision> &b) { return a / b; }}

                                                  }),
               unaryOperatorMap({{
                                         '!', [](const MpInt<bytePrecision> &a) { return a.factorial(); }
                                 }}) {

    };
    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ METHODS ---------------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
private:
    /**
     * @brief Trim white spaces from left of string.
     * @param s String to be trimmed.
     */
    inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }
    /**
     * @brief Trim white spaces from right of string.
     * @param s String to be trimmed.
    */
    inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    /**
     * @brief Trim white spaces of string.
     * @param s String to be trimmed.
     * @return Trimmed string.
    */
    inline std::string trim(const std::string &s) {
        auto copy = s;
        rtrim(copy);
        ltrim(copy);
        return copy;
    }

    /**
     * @brief Split terminal string according to delimiter. String must be checked with proper regex to call this method on it.
     * @param s Terminal input.
     * @param delim Operator delimiter.
     * @return Array of two string terms.
     */
    std::array<std::string, 2> split(const std::string &s, char delim) {
        std::array<std::string, 2> result;
        auto ignoreSubChar = delim == '-';
        auto trimmed = trim(s);
        auto termStart = true;
        int termIndex = 0;

        for (auto i = 0; i < s.length(); i++) {
            if (s[i] == delim) {
                if (ignoreSubChar && termStart) {
                    result[termIndex] = '-';
                    continue;
                } else {
                    termIndex++;
                    termStart = true;
                    continue;
                }
            }
            result[termIndex] += s[i];
            termStart = false;
        }
        return result;
    }

    /**
     * @brief Find term from string or return "nullopt" if result is from bank - above index.
     * @param term String term to be resolved.
     * @return Optional Term.
     */
    std::optional<MpInt<bytePrecision>> getTerm(const std::string &term) {
        if (term.starts_with('$')) {
            auto index = term[1] - '1';
            if (bank.getResults().size() <= index) {
                return std::nullopt;
            }
            return bank.getResults()[index];
        } else {
            return MpInt<bytePrecision>(std::stoll(term));
        }
    }

    /**
     * @brief Check if command is "exit"
     * @param command Terminal input.
     * @return command == "exit"
     */
    bool isExitCommand(const std::string &command) {
        return command == "exit";
    }

    /**
     * @brief Check if command is "bank"
     * @param command Terminal input.
     * @return command == "bank"
    */
    bool isBankCommand(const std::string &command) {
        return command == "bank";
    }

    /**
     * @brief Process MpInt calculation
     * @param command Terminal input.
     */
    void processCalculation(const std::string &command) {
        std::array<std::string, 2> binaryTerms;
        std::string unaryTerm;
        char oper;
        std::array<MpInt<bytePrecision>, 2> mpIntTerms;
        if (std::regex_match(command, ADDITION_REG)) {
            binaryTerms = split(command, '+');
            oper = '+';
        } else if (std::regex_match(command, SUBTRACT_REG)) {
            binaryTerms = split(command, '-');
            oper = '-';
        } else if (std::regex_match(command, MULTIPLY_REG)) {
            binaryTerms = split(command, '*');
            oper = '*';
        } else if (std::regex_match(command, DIVIDE_REG)) {
            binaryTerms = split(command, '/');
            oper = '/';
        } else if (std::regex_match(command, FACTORIAL_REG)) {
            oper = '!';
            unaryTerm = trim(command);
            unaryTerm = unaryTerm.substr(0, unaryTerm.length() - 1);
        } else {
            std::cout << "Neznámý výraz." << std::endl;
            return;
        }
        try {
            MpInt<bytePrecision> result;
            if (oper == '!') {
                auto term = getTerm(unaryTerm);
                if (term.has_value()) {
                    result = this->unaryOperatorMap[oper](term.value());
                } else {
                    std::cout << "Neznámý výraz." << std::endl;
                    return;
                }
            } else {
                auto term1 = getTerm(binaryTerms[0]);
                auto term2 = getTerm(binaryTerms[1]);
                if (term1.has_value() && term2.has_value()) {
                    result = this->binaryOperatorMap[oper](term1.value(), term2.value());
                } else {
                    std::cout << "Neznámý výraz." << std::endl;
                    return;
                }
            }
            bank.push(result);
            std::cout << result.toDecimal() << std::endl;
        } catch (MpIntException<MpInt<MP_INT_UNLIMITED>> &e) {
            std::cout << "Došlo k přetečení čísla." << std::endl;
            std::cout << e.overflow.toDecimal() << std::endl;
        }
    }

    /**
     * @brief Print all items of bank on terminal.
     */
    void printBank() {
        if (bank.getResults().empty()) {
            std::cout << "Banka je prázdná!" << std::endl;
        } else {
            std::size_t index = 0;
            for (MpInt<bytePrecision> item: bank.getResults()) {
                std::cout << "$" << ++index << ": " << item.toDecimal() << std::endl;
            }
        }
    }

    /**
     * @brief Print greeting to console.
     */
    void printGreeting() {
        std::cout
                << "--------------------------------------------------------------------------------------------------------------------\n"
                   "--------------------------------------------------------------------------------------------------------------------\n"
                   "--------------------@@@@@@@@@@@@@@@@@@------------------------------------------------------------------------------\n"
                   "--------------------@@@@@@@@@@@@@@@@@@------------------------------------------------------------------------------\n"
                   "--------------------@@@-----------@@@@------------------------------------------------------------------------------\n"
                   "--------------------@@@---------------------------------------------------------------------------------------------\n"
                   "--------------------@@@---------------------------------------------------------------------------------------------\n"
                   "--------------------@@@--------------------------@@@-----------------------@@@--------------------------------------\n"
                   "--------------------@@@--------------------------@@@-----------------------@@@--------------------------------------\n"
                   "--------------------@@@--------------------------@@@-----------------------@@@--------------------------------------\n"
                   "--------------------@@@-----------------@@@@@@@@@@@@@@@@@@@@@-----@@@@@@@@@@@@@@@@@@@@@-----------------------------\n"
                   "--------------------@@@-----------------@@@@@@@@@@@@@@@@@@@@@-----@@@@@@@@@@@@@@@@@@@@@-----------------------------\n"
                   "--------------------@@@--------------------------@@@-----------------------@@@--------------------------------------\n"
                   "--------------------@@@--------------------------@@@-----------------------@@@--------------------------------------\n"
                   "--------------------@@@--------------------------@@@-----------------------@@@--------------------------------------\n"
                   "--------------------@@@---------------------------------------------------------------------------------------------\n"
                   "--------------------@@@---------------------------------------------------------------------------------------------\n"
                   "--------------------@@@---------------------------------------------------------------------------------------------\n"
                   "--------------------@@@-----------@@@@------------------------------------------------------------------------------\n"
                   "--------------------@@@@@@@@@@@@@@@@@@------------------------------------------------------------------------------\n"
                   "--------------------@@@@@@@@@@@@@@@@@@------------------------------------------------------------------------------\n"
                   "--------------------------------------------------------------------------------------------------------------------\n"
                   "-------------@----@-@-----@-@------@@@-@-----@-@@@-@@@@@-@@@@@-@@@@@@-----------------------------------------------\n"
                   "-------------@----@-@----@@-@-------@--@@---@@--@----@---@------@---@-----------------------------------------------\n"
                   "-------------@----@-@---@-@-@-------@--@-@-@-@--@----@---@------@---@-----------------------------------------------\n"
                   "-------------@----@-@--@--@-@-------@--@--@--@--@----@---@@@@---@---@-----------------------------------------------\n"
                   "-------------@----@-@-@---@-@-------@--@-----@--@----@---@------@---@-----------------------------------------------\n"
                   "-------------@----@-@@----@-@-------@--@-----@--@----@---@------@---@-----------------------------------------------\n"
                   "-------------@@@@@@-@-----@-@@@@@@-@@@-@-----@-@@@---@---@@@@@-@@@@@@-----------------------------------------------\n"
                   "--------------------------------------------------------------------------------------------------------------------\n"
                   "--------------------------------------------------------------------------------------------------------------------\n"
                   "------------------------------@@@@@-------@-------@-----@@@@@-@---@-@-----------@-------@@@@@-@@@@@-@@@@@-----------\n"
                   "------------------------------@---@------@-@------@-----@---@-@---@-@----------@-@--------@---@---@-@---@-----------\n"
                   "------------------------------@---------@---@-----@-----@-----@---@-@---------@---@-------@---@---@-@@@@@-----------\n"
                   "------------------------------@--------@@@@@@@----@-----@-----@---@-@--------@@@@@@@------@---@---@-@@--------------\n"
                   "------------------------------@-------@-------@---@-----@-----@---@-@-------@-------@-----@---@---@-@-@-------------\n"
                   "------------------------------@---@--@---------@--@-----@---@-@---@-@------@---------@----@---@---@-@--@------------\n"
                   "------------------------------@@@@@-@-----------@-@@@@@-@@@@@-@@@@@-@@@@@-@-----------@---@---@@@@@-@---@-----------\n"
                   "--------------------------------------------------------------------------------------------------------------------\n"
                   "--------------------------------------------------------------------------------------------------------------------"
                << std::endl;
        std::cout << "Vítejte v kalkulačce na neomezená čísla." << std::endl;
        std::cout << "Zadejte jednoduchý matematický výraz s nejvýše jednou operací +, -, *, / nebo !." << std::endl;
    }

    /**
     * @brief Receive command from console.
     * @return User command.
     */
    std::string getCommand() {
        std::string command;
        std::getline(std::cin, command);
        return trim(command);
    }

public:
    /**
     * @brief Run application loop.
     */
    void run() {
        printGreeting();
        std::cout << std::endl;
        std::cout << std::endl;
        while (true) {
            std::cout << "> ";
            auto command = getCommand();
            if (isExitCommand(command)) {
                break;
            } else if (isBankCommand(command)) {
                printBank();
            } else {
                processCalculation(command);
            }
        }
        std::cout << "Děkujeme za využití naší kalkulačky! :)" << std::endl;
    }
};