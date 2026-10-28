// Reads "opnd1 op opnd2" expressions, stores them in a heap-allocated,
// fixed-size array of BinOp objects, evaluates them and writes
// "expression -> value" lines to a text file.

#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

// Step 1 was a plain struct { double lhs; char op; double rhs; };
// Step 2 (this): a class that hides the tuple behind getters/setters

class BinOp
{
public:
    BinOp() = default;

    BinOp(double lhs, char op, double rhs) : lhs_(lhs), rhs_(rhs)
    {
        setOp(op); // validates
    }

    double getLhs() const { return lhs_; }
    double getRhs() const { return rhs_; }
    char getOp() const { return op_; }

    void setLhs(double v) { lhs_ = v; }
    void setRhs(double v) { rhs_ = v; }
    void setOp(char op)
    {
        if (op != '+' && op != '-' && op != '*' && op != '/')
        {
            throw std::invalid_argument(std::string("unsupported operator '") + op + "'");
        }
        op_ = op;
    }

    // Throws std::domain_error on division by zero.
    double evaluate() const
    {
        switch (op_)
        {
        case '+':
            return lhs_ + rhs_;
        case '-':
            return lhs_ - rhs_;
        case '*':
            return lhs_ * rhs_;
        case '/':
            if (rhs_ == 0.0)
                throw std::domain_error("division by zero");
            return lhs_ / rhs_;
        }
        throw std::logic_error("invalid operator state"); // unreachable
    }

    std::string toString() const
    {
        std::ostringstream oss;
        oss << lhs_ << ' ' << op_ << ' ' << rhs_;
        return oss.str();
    }

private:
    double lhs_ = 0.0;
    double rhs_ = 0.0;
    char op_ = '+';
};

// ---------------------------------------------------------------------------
// Approach i): >> directly on std::istream
// Returns true if an expression was read, false on EOF.
// ---------------------------------------------------------------------------
bool readDirect(std::istream &in, BinOp &out)
{
    while (true)
    {
        std::cout << "expr> ";
        double a = 0, b = 0;
        char op = 0;

        in >> a >> op >> b;

        if (in.eof())
            return false; // Ctrl+D / Ctrl+Z (or EOF mid-expression)

        if (!in)
        { // ios::operator! : a failed extraction
            std::cerr << "  Invalid input, expected: <number> <+|-|*|/> <number>\n";
            in.clear();                                                   // reset the error flags
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // drop the bad line
            continue;
        }
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // drop rest of line

        try
        {
            out = BinOp(a, op, b);
            return true;
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "  " << e.what() << "\n";
        }
    }
}

// ---------------------------------------------------------------------------
// Approach ii): std::getline + std::istringstream
// Returns true if an expression was read, false on EOF or blank line.
// ---------------------------------------------------------------------------
bool readLine(std::istream &in, BinOp &out)
{
    std::string line;
    while (true)
    {
        std::cout << "expr> ";
        if (!std::getline(in, line))
            return false; // EOF
        if (line.find_first_not_of(" \t\r") == std::string::npos)
            return false; // blank = done

        std::istringstream iss(line);
        double a = 0, b = 0;
        char op = 0;

        if (!(iss >> a >> op >> b))
        {
            std::cerr << "  Invalid input, expected: <number> <+|-|*|/> <number>\n";
            continue;
        }
        iss >> std::ws; // skip trailing blanks
        if (!iss.eof())
        {
            std::cerr << "  Unexpected trailing characters\n";
            continue;
        }

        try
        {
            out = BinOp(a, op, b);
            return true;
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "  " << e.what() << "\n";
        }
    }
}

int main(int argc, char *argv[])
{
    const int approach = (argc > 1) ? std::stoi(argv[1]) : 2;
    const std::string outPath = (argc > 2) ? argv[2] : "results.txt";

    constexpr std::size_t CAPACITY = 5;
    BinOp *ops = new BinOp[CAPACITY]; // fixed-size array on the heap
    std::size_t count = 0;

    std::cout << "Enter up to " << CAPACITY << " expressions (e.g. 3.5 * 2). "
              << "End with blank line / EOF.\n";

    while (count < CAPACITY)
    {
        bool ok = (approach == 1) ? readDirect(std::cin, ops[count])
                                  : readLine(std::cin, ops[count]);
        if (!ok)
            break;
        ++count;
    }

    std::ofstream file(outPath);
    if (!file)
    {
        std::cerr << "Cannot open '" << outPath << "' for writing\n";
        delete[] ops;
        return 1;
    }

    for (std::size_t i = 0; i < count; ++i)
    {
        file << ops[i].toString() << " -> ";
        try
        {
            file << ops[i].evaluate();
        }
        catch (const std::exception &e)
        {
            file << "error (" << e.what() << ")";
        }
        file << '\n';
    }

    std::cout << "Wrote " << count << " result(s) to " << outPath << "\n";
    delete[] ops; // matches new[]
    return 0;
}