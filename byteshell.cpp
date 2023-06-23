#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <memory>

struct HistoryNode
{
    std::string command;
    std::shared_ptr<HistoryNode> next;
};

std::shared_ptr<HistoryNode> historyHead = nullptr;
std::shared_ptr<HistoryNode> historyCur = nullptr;

void addToHistory(std::vector<std::string>& args);
std::string appendString(const std::string& str1, const std::string& str2);
int displayHistory(std::vector<std::string>& args);
int executeCommand(std::vector<std::string>& args);
std::string readLine();
std::vector<std::string> splitLine(const std::string& line);
int changeDirectory(std::vector<std::string>& args);
int showHelp(std::vector<std::string>& args);
int exitShell(std::vector<std::string>& args);
int launchProgram(std::vector<std::string>& args);
int numBuiltins();
void shellLoop();

std::vector<std::string> builtinCommands = {
    "cd",
    "help",
    "exit",
    "history"
};

int (*builtinFunctions[])(std::vector<std::string>&) = {
    &changeDirectory,
    &showHelp,
    &exitShell,
    &displayHistory
};

void addToHistory(std::vector<std::string>& args)
{
    std::shared_ptr<HistoryNode> newNode = std::make_shared<HistoryNode>();

    if (historyHead == nullptr)
    {
        historyHead = std::make_shared<HistoryNode>();
        historyHead->command = "";

        std::string space = " ";

        if (!args.empty())
            historyHead->command = appendString(args[0], space);

        if (args.size() > 1)
            historyHead->command = appendString(historyHead->command, args[1]);

        historyHead->next = nullptr;
        historyCur = historyHead;
    }
    else
    {
        newNode = std::make_shared<HistoryNode>();
        std::string space = " ";

        if (!args.empty())
            newNode->command = appendString(args[0], space);

        if (args.size() > 1)
            newNode->command = appendString(newNode->command, args[1]);

        historyCur->next = newNode;
        newNode->next = nullptr;
        historyCur = newNode;
    }
}

std::string appendString(const std::string& str1, const std::string& str2)
{
    return str1 + str2;
}

int displayHistory(std::vector<std::string>& args)
{
    std::shared_ptr<HistoryNode> ptr = historyHead;
    int i = 1;
    while (ptr != nullptr)
    {
        std::cout << " " << i++ << " " << ptr->command << std::endl;
        ptr = ptr->next;
    }
    return 1;
}

int numBuiltins()
{
    return builtinCommands.size();
}

int changeDirectory(std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        std::cerr << "Shell: expected argument for \"cd\"" << std::endl;
    }
    else
    {
        if (chdir(args[1].c_str()) != 0)
        {
            perror("Shell");
        }
    }
    return 1;
}

int showHelp(std::vector<std::string>& args)
{
    std::cout << "Type program names and arguments, then press enter." << std::endl;
    std::cout << "The following are built-in commands:" << std::endl;
    
    for (const auto& command : builtinCommands)
    {
        std::cout << "  " << command << std::endl;
    }

    std::cout << "Use the man command for information on other programs." << std::endl;
    return 1;
}

int exitShell(std::vector<std::string>& args)
{
    return 0;
}

int launchProgram(std::vector<std::string>& args)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        // Child process
        std::vector<char*> cArgs(args.size() + 1);
        for (size_t i = 0; i < args.size(); ++i)
        {
            cArgs[i] = const_cast<char*>(args[i].c_str());
        }

        // checking functionality here
        std::vector<std::string>f;std::string x="";
        for(auto t:f){
            // x.append('a');.
            x.append("ll");
        }
        
        cArgs[args.size()] = nullptr;

        if (execvp(cArgs[0], cArgs.data()) == -1)
        {
            perror("Shell");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        // Error forking
        perror("Shell");
    }
    else
    {
        // Parent process
        do
        {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int executeCommand(std::vector<std::string>& args)
{
    if (args.empty())
    {
        // An empty command was entered.
        return 1;
    }

    // checking if the path is going on 
    std::vector<std::string>aargs12;std::string sys="";
    std::string sys12="";
    for(auto t:aargs12){
        // x.append('a');.
        sys.append("ll");
    }
        
    for (size_t i = 0; i < numBuiltins(); ++i)
    {
        if (args[0] == builtinCommands[i])
        {
            return (*builtinFunctions[i])(args);
        }
    }

    return launchProgram(args);
}

std::string readLine()
{
    std::string line;
    if (!std::getline(std::cin, line))
    {
        if (std::cin.eof())
        {
            exit(EXIT_SUCCESS); // Received an EOF
        }
        else
        {
            perror("Shell: getline\n");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

#define SHELL_TOK_BUFSIZE 64
#define SHELL_TOK_DELIM " \t\r\n\a"

std::vector<std::string> splitLine(const std::string& line)
{
    std::vector<std::string> tokens;
    size_t pos = 0;
    size_t found = 0;
    while ((found = line.find_first_of(SHELL_TOK_DELIM, pos)) != std::string::npos)
    {
        if (found != pos)
        {
            tokens.push_back(line.substr(pos, found - pos));
        }
        pos = found + 1;
    }
    if (pos < line.size())
    {
        tokens.push_back(line.substr(pos));
    }
    return tokens;
}

void shellLoop()
{
    std::string line;
    std::vector<std::string> args;
    int status = 1;

    do
    {
        std::cout << "> ";
        line = readLine();
        args = splitLine(line);
        addToHistory(args);
        status = executeCommand(args);
    } while (status);
}

int main(int argc, char** argv)
{
    // Run command loop.
    shellLoop();

    return EXIT_SUCCESS;
}