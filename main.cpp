#include <iostream>
#include <fstream>

static const int kBufferSize = 20;
static const int kMinimalArguments = 2;

static const char* kLongLinesArg = "--lines=";
static const size_t kLongLinesArgLen = strlen(kLongLinesArg);

static const char* kLongDelimiterArg = "--delimiter=";
static const size_t kLongDelimiterArgLen = strlen(kLongDelimiterArg);

static const char* kLongTailArg = "--tail";
static const size_t kLongTailArgLen = strlen(kLongTailArg);

static const char* kShortLinesArg = "-l";
static const char* kShortDelimiterArg = "-d";
static const char* kShortTailArg = "-t";
static const size_t kShortArgLen = strlen(kShortLinesArg);

struct Parameters {
    char file_name[kBufferSize];
    uint64_t lines = 0;
    char delimiter = '\n';
    bool tail = false;
};

void ParseArguments(const int& argc, char* argv[], Parameters& params);
void PrintFirstNLines(const Parameters& params, std::ifstream& file);
void PrintLastNLines(const Parameters& params, std::ifstream& file);
void OpenFile(const char* const file_name, std::ifstream& file);
void Print(const Parameters& params);
void PrintAll(std::ifstream& file);
void ShowUsage();
char GetEscapeSequence(const char* sequence);
char GetDelimiter(const char* str);
bool IsNumber(const char* str);
uint64_t GetLines(const char* ptr);


int main(int argc, char *argv[]) {
    Parameters params;
    ParseArguments(argc, argv, params);
    Print(params);
 
    return EXIT_SUCCESS;
}

void Print(const Parameters& params) {
    std::ifstream file;
    OpenFile(params.file_name, file);
    
    if (params.tail) {
        PrintLastNLines(params, file);
    } else if (params.lines) {
        PrintFirstNLines(params, file);
    } else {
        PrintAll(file);
    }
    
    std::cout << '\n';
}

void PrintAll(std::ifstream& file) {
    char ch;
    
    while ((ch = file.get()) != EOF) {
        std::cout << ch;
    }
}

void PrintFirstNLines(const Parameters& params, std::ifstream& file) {
    uint64_t lines_count = 0;
    char ch;
    
    while ((ch = file.get()) != EOF && lines_count < params.lines) {
        if (ch == params.delimiter) {
            lines_count++;
        }

        std::cout << ch;
    }
}

void PrintLastNLines(const Parameters& params, std::ifstream& file) {
    char ch;
    
    file.seekg(0, std::ios::end);
    uint64_t size = file.tellg();
    
    uint64_t lines_count = UINT64_MAX;
    if (params.lines) {
        lines_count = 0;
    }
    
    for (uint64_t i = 1; i <= size; i++) {
        file.seekg(-i, std::ios::end);
        ch = file.get();
        
        if (params.lines) {
            if (lines_count == params.lines) {
                break;
            }
            if (ch == params.delimiter) {
                lines_count++;
            }
        }
            
        if (ch == '\n' || lines_count == params.lines || i == size) {
            if (i == size) {
                std::cout << ch;
            }
            
            if (file.tellg() != size)
                ch = file.get();
            
            while (ch != '\n') {
                std::cout << ch;
                
                if (file.tellg() == size) {
                    break;
                }
                ch = file.get();
            }
            
            std::cout << '\n';
        }
    }
}

void OpenFile(const char* const file_name, std::ifstream& file) {
    if (file.is_open()) {
        ShowUsage();
    }
    
    file.open(file_name);
    if (file.fail()) {
        std::cerr << "File " << file_name << " wasn't found\n";
        exit(EXIT_FAILURE);
    }
}

char GetEscapeSequence(const char* sequence) {
    if (*sequence == '\\') {
        sequence++;
    } else {
        ShowUsage();
    }
    
    switch (*sequence) {
        case 't':
            return '\t';

        case 'b':
            return '\b';

        case 'n':
            return '\n';

        case 'r':
            return '\r';

        case 'f':
            return '\f';

        case 'v':
            return '\v';

        case 'a':
            return '\a';
            
        default:
            return 0;
    }
}

bool IsNumber(const char* str) {
    while (*str != '\0') {
        if (!isdigit(*str++)) {
            return false;
        }
    }
    
    return true;
}

void ShowUsage() {
    std::cout << '\n';
    std::cout << "Warning: this programm works only on MacOS\n";
    std::cout << '\n';
    std::cout << "Usage: ./PrintFile [OPTION] filename.txt" << '\n';
    std::cout << "Options:\n";
    std::cout << "-l n, --lines=n : print n lines\n";
    std::cout << "-t, --tail : start printing from end of the file\n";
    std::cout << "-d c, --delimiter=c : set delimiter = c\n";
    std::cout << '\n';
    std::cout << "For example: ./PrintFile --tail --lines=10 filename.txt" << '\n';

    exit(EXIT_SUCCESS);
}

char GetDelimiter(const char* str) {
    
    
    if (strlen(str) == 1) {
        return *str;
    } else if (strlen(str) == 2) {
        return GetEscapeSequence(str);
    } else {
        ShowUsage();
    }
    
    return 0;
}

uint64_t GetLines(const char* ptr) {
    if (!IsNumber(ptr)) {
        ShowUsage();
    }
    
    if (strlen(ptr) >= 19) {
        std::cerr << "Number of lines must be from 0 to 1000000000000000000\n";
        exit(EXIT_FAILURE);
    }
    
    return strtoll(ptr, nullptr, 10);
}

void ParseArguments(const int& argc, char* argv[], Parameters& params) {
    if (argc < kMinimalArguments) {
        ShowUsage();
    }
    
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], kShortLinesArg, kShortArgLen) == 0) {
            params.lines = GetLines(argv[i] + kShortArgLen+1);
            i++; // Skip processed argument
        } else if (strncmp(argv[i], kLongLinesArg, kLongLinesArgLen) == 0) {
            params.lines = GetLines(argv[i] + kLongLinesArgLen);
        } else if (strncmp(argv[i], kShortDelimiterArg, kShortArgLen) == 0) {
            params.delimiter = GetDelimiter(argv[i] + kShortArgLen+1);
            i++; // Skip processed argument
        } else if (strncmp(argv[i], kLongDelimiterArg, kLongDelimiterArgLen) == 0) {
            params.delimiter = GetDelimiter(argv[i] + kLongDelimiterArgLen);
        } else if (strncmp(argv[i], kShortTailArg, kShortArgLen) == 0 ||
                   strncmp(argv[i], kLongTailArg, kLongTailArgLen) == 0) {
            params.tail = true;
        } else {
            strncpy(params.file_name, argv[i], kBufferSize);
        }
    }
}
