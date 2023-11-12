#include <string>
#include <tuple>
#include <stack>
#include <optional>
#include <iostream>
class TopDownRegExprRecognition
{

    public:
    using IndexStatus = std::tuple<size_t,size_t>;
    TopDownRegExprRecognition(std::string inputStr, std::string regStr)
    :inputStr(inputStr), regStr(regStr), curInputIndex(0), curRegIndex(0)
    {
        
    }

    bool recognition()
    {
        regular_expression();
        printStrInfo();
        return isMatch();
    }
    //regular_expressions ---> compound_re*
    bool regular_expression()
    {
        std::cout << std::endl << "regular_expression\n";
        printStrInfo();
        
        bool ret = false;
        IndexStatus startStatus, curStatus;
        startStatus = curStatus = getCurIndex();
        while(more() && (ret = compound_re()))
        {
            curStatus = getCurIndex();
        }
        
        return startStatus != curStatus;
    }
    //compound_re ---> repeat_re | simple_re
    bool compound_re()
    {
        std::cout << std::endl << "compound_re\n";
        printStrInfo();
        IndexStatus startStatus = getCurIndex();
        bool ret = repeat_re();
        if (ret) return true;
        restoreIndex(startStatus);

        ret = simple_re();
        return getCurIndex() != startStatus ;
    }

    bool repeat_re()
    {
        std::cout << std::endl << "repeat_re\n";
        printStrInfo();

        IndexStatus startStatus = getCurIndex();
        auto afterStarIndex = hasStar();
        
        if (afterStarIndex == std::nullopt) return false;
        
        //has star
        //1) zero star
        curRegIndex = afterStarIndex.value();
        bool ret = regular_expression();
        if (ret) return ret;
        restoreIndex(startStatus);
        //2) one plus start
        
        size_t maxStarNum = 0;
        size_t simpleTokenLen = 0;
        while (curInputIndex < inputStr.length())
        {
            ret = simple_re();
            if (simpleTokenLen == 0)
                simpleTokenLen = curInputIndex - std::get<0>(startStatus);
            if (ret)
            {
                maxStarNum ++;
                restoreRegIndex(std::get<1>(startStatus));
            }
            else
            {
                break;
            }
        }
        restoreIndex(startStatus);
        IndexStatus endStatus = getCurIndex();
        for(size_t curStarNum = 1 ; curStarNum <= maxStarNum ; curStarNum ++)
        {
                curInputIndex = std::get<0>(startStatus) + simpleTokenLen * curStarNum;
                curRegIndex = afterStarIndex.value();
                regular_expression();
                if (curInputIndex > std::get<0>(endStatus))
                {
                    endStatus = getCurIndex();
                }
        }

        if (endStatus != startStatus)
        {
            curInputIndex = std::get<0>(endStatus);
            curRegIndex = std::get<1>(endStatus);

            return true;
        }
        restoreIndex(startStatus);
        return false;
#if 0
        while(1){
            
            ret = simple_re();
            if (ret)
            {
                //restoreRegIndex(std::get<1>(startStatus));
                eatRegStr('*');
                ret = regular_expression();
//                if (isMatch()) 
//                    ret = true;
                if (ret )
                {
                    break;
                }
                else
                {
                    restoreRegIndex(std::get<1>(startStatus));

                }
            }
            else
            {
                restoreIndex(startStatus);
                break;
            }
        }
#endif
        return ret;
    }
    std::optional<size_t> hasStar()
    {
        size_t tmpIndex = curRegIndex;
        if (regStr[tmpIndex] != ')' && regStr[tmpIndex] != '(' && regStr[tmpIndex] != '*' && tmpIndex + 1 < regStr.length() 
                && regStr[tmpIndex + 1] == '*')
        {
            return tmpIndex + 2;
        }
        if (regStr[tmpIndex] == '(')
        {
            size_t bracketNum = 1;
            while(++ tmpIndex < regStr.length())
            {
                if (regStr[tmpIndex] == '(') bracketNum ++;
                if (regStr[tmpIndex] == ')')
                {
                    if (-- bracketNum == 0)
                    {
                        
                        break;
                    }
                }
            }
            if (++ tmpIndex < regStr.length() && regStr[tmpIndex] == '*')
            {
                std::cout << "has start" << std::endl;
                return tmpIndex + 1;
            }
        }
        return std::nullopt;
    }

    bool simple_re()
    {
        std::cout << std::endl << "simple_re\n";
        printStrInfo();
        int ret = false;
        IndexStatus startStatus = getCurIndex();
        if (eatInputStr())
        {
            return true;
        }

        if (eatRegStr('('))
        {
            ret = regular_expression();
            if (eatRegStr(')'))
            {
                ret = true;
            }
            else
            {
                ret = false;
                restoreIndex(startStatus);
            }
        }
        return ret;
    }
    private:
    bool isMatch()
    {
        return regStr.length() == curRegIndex && inputStr.length() == curInputIndex;
    }
    bool more()
    {
        if (curRegIndex < regStr.length() && curInputIndex < inputStr.length()) return true;
        return false;
    }

    bool eatRegStr(char ch)
    {
        if (getRegToken() == ch)
        {
            curRegIndex ++;
            return true;
        }
        return false;
    }
    void restoreRegIndex(size_t indexStatus)
    {
        curRegIndex = indexStatus;
    }
    void restoreIndex(IndexStatus indexStatus)
    {
        curInputIndex = std::get<0>(indexStatus);
        curRegIndex = std::get<1>(indexStatus);
    }

    bool eatInputStr()
    {
        if (getInputToken() == getRegToken())
        {
            curInputIndex ++;
            curRegIndex ++;
            return true;
        }
        return false;
    }

    char getInputToken()
    {
         return inputStr[curInputIndex];
    }

    char getRegToken()
    {
        return regStr[curRegIndex];
    }
    std::tuple<size_t,size_t> getCurIndex()
    {
        return {curInputIndex, curRegIndex};
    }
    void printStrInfo()
    {
        std::cout << inputStr << "  | " << regStr << std::endl;
        for(decltype(inputStr.size()) i = 0 ; i <= inputStr.length(); i ++)
        {
            if (i != curInputIndex)
            {
                std::cout << " ";
            }
            else
            {
                std::cout << "^";
            }
        }
        std::cout << " | ";

        for(decltype(regStr.size()) i = 0 ; i <= regStr.length(); i ++)
        {
            if (i != curRegIndex)
            {
                std::cout << " ";
            }
            else
            {
                std::cout << "^";
            }
        }
        std::cout << std::endl;

    }
    std::string inputStr;
    std::string regStr;
    size_t curInputIndex;
    size_t curRegIndex;
};