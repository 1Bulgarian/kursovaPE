#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iomanip>
#include <functional>

enum class Currency {
    BGN,
    EUR,
    USD,
    GBP,
    TRY,
};

std::string currencyToString(Currency currency) {
    switch (currency) {
        case Currency::BGN: return "BGN";
        case Currency::EUR: return "EUR";
        case Currency::USD: return "USD";
        case Currency::GBP: return "GBP";
        case Currency::TRY: return "TRY";
        default: throw std::invalid_argument("Unsupported currency.");
    }
}

struct CurrencyPair {
    Currency from;
    Currency to;

    bool operator==(const CurrencyPair& other) const {
        return from == other.from && to == other.to;
    }
};

namespace std {
    template <>
    struct hash<CurrencyPair> {
        std::size_t operator()(const CurrencyPair& pair) const {
            return std::hash<int>()(static_cast<int>(pair.from)) ^
                   (std::hash<int>()(static_cast<int>(pair.to)) << 1);
        }
    };
}

class ExchangeRates {
private:
    std::unordered_map<CurrencyPair, double> rates;

public:
    void setRate(Currency from, Currency to, double rate) {
        if (rate <= 0) throw std::invalid_argument("Rate must be positive.");
        rates[{from, to}] = rate;
        rates[{to, from}] = 1.0 / rate;
    }

    double getRate(Currency from, Currency to) const {
        if (from == to) return 1.0;
        auto it = rates.find({from, to});
        if (it != rates.end()) {
            return it->second;
        }
        throw std::invalid_argument("Exchange rate not found.");
    }
};

class Money {
private:
    double amount;
    Currency currency;

public:
    Money(double amount, Currency currency) : amount(amount), currency(currency) {
        if (amount < 0) throw std::invalid_argument("Стойността не може да бъде негативна.");
    }

    double getAmount() const { return amount; }
    Currency getCurrency() const { return currency; }

    Money convertTo(Currency targetCurrency, const ExchangeRates& rates) const {
        double rate = rates.getRate(currency, targetCurrency);
        return Money(amount * rate, targetCurrency);
    }

    bool operator==(const Money& other) const {
        return amount == other.amount && currency == other.currency;
    }

    bool operator<(const Money& other) const {
        if (currency != other.currency) {
            throw std::invalid_argument("Не може да се сравняват стойности в различни валути.");
        }
        return amount < other.amount;
    }

    friend std::ostream& operator<<(std::ostream& os, const Money& money) {
        os << std::fixed << std::setprecision(7) << money.amount << " " << currencyToString(money.currency);
        return os;
    }
};

int main() {
    ExchangeRates rates;
    //BGN to EUR
    rates.setRate(Currency::BGN, Currency::EUR, 0.511292);
    //BGN to USD
    rates.setRate(Currency::BGN, Currency::USD, 0.53758337);
    //BGN to TRY
    rates.setRate(Currency::BGN, Currency::TRY, 19.18541);
    //EUR to USD
    rates.setRate(Currency::USD, Currency::EUR, 0.95067787);
    //TRY to EUR
    rates.setRate(Currency::TRY, Currency::EUR, 0.026648383);
    //TRY to USD
    rates.setRate(Currency::TRY, Currency::USD, 0.028026054);
    //GBP to USD
    rates.setRate(Currency::GBP, Currency::USD, 1.2499343);
    //GBP to EUR
    rates.setRate(Currency::GBP, Currency::EUR, 1.188293);
    //GBP to BGN
    rates.setRate(Currency::GBP, Currency::BGN, 2.3242501);
    //GBP to TRY
    rates.setRate(Currency::GBP, Currency::TRY, 44.343556);

    Money amountBGN(100, Currency::BGN);
    Money amountEUR = amountBGN.convertTo(Currency::EUR, rates);
    Money amountTRY = amountEUR.convertTo(Currency::TRY, rates);
    Money amountUSD = amountTRY.convertTo(Currency::USD, rates);
    Money amountGBP = amountUSD.convertTo(Currency::GBP, rates);
    Money suma(100, Currency::EUR);

    std::cout << "Amount in BGN: " << amountBGN << std::endl;
    std::cout << "Converted to EUR: " << amountEUR << std::endl;
    std::cout << "Converted to TRY: " << amountTRY << std::endl;
    std::cout << "Converted to USD: " << amountUSD << std::endl;
    std::cout << "Converted to GBP: " << amountGBP << std::endl;
    std::cout << "Converted to TRY: " << suma.convertTo(Currency::BGN, rates).convertTo(Currency::EUR, rates).convertTo(Currency::TRY, rates).convertTo(Currency::GBP, rates) << std::endl;

    Money anotherAmountEUR(51.1291881, Currency::EUR);
    if (amountEUR == anotherAmountEUR) {
        std::cout << "The amounts are equal." << std::endl;
    } else {
        std::cout << "The amounts are not equal." << std::endl;
    }

    return 0;
}
