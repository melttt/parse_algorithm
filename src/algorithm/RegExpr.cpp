#include "RegExpr.h"


TopDownRegExprRecognition::TopDownRegExprRecognition(std::string inputStr, std::string regStr)
:inputStr(inputStr), regStr(regStr)
{
}
bool TopDownRegExprRecognition::recognition()
{
    RegExprState inputState{regStr, inputStr, 0, 0};
    bool ret = false;
    
    for(auto outputState : parseAlternativesExpr(inputState))
    {
        if (outputState != inputState && !outputState.more())
        {
            std::cout << "matching string : " << inputStr.substr(0, outputState.curStrIndex) << std::endl;
            ret = true;
        }
    }
    return ret;    
    
    
    
}
std::set<TopDownRegExprRecognition::RegExprState> TopDownRegExprRecognition::parseAlternativesExpr(RegExprState origState)
{
    std::set<RegExprState> resultStates = {};
    std::cout << "parseAlternativesExpr" <<std::endl;
    origState.printStrInfo();

    for(auto re : parseMultiRegularExpr(origState.splitAlternatives()))
    {
        if (!re.more())
        {
            resultStates.insert({origState.curRegExpr, origState.curStr, origState.curRegExpr.length(), re.curStrIndex});   
        }
    }
    return resultStates;
}

std::set<TopDownRegExprRecognition::RegExprState> TopDownRegExprRecognition::parseMultiRegularExpr(std::set<RegExprState> origStates)
{
    
    std::set<RegExprState> resultStates = {};
    for(const auto& curOrigState : origStates)
    {
        std::cout << "parseRegularExpr" << std::endl;
        curOrigState.printStrInfo();
        RegExprState inputState{curOrigState};
        if (!inputState.more())
        {
            resultStates.insert(inputState);
            continue;
        }
        std::set<RegExprState> changeStates{};
        for(auto &curState : parseMultiCompoundExpr(inputState))
        {
            if (curState != inputState)
            {
                changeStates.insert(curState);
            }
            else
            {
                resultStates.insert(curState);
            }
        }            
        resultStates.merge(parseMultiRegularExpr(changeStates));
    }
    return resultStates;
}


std::set<TopDownRegExprRecognition::RegExprState> TopDownRegExprRecognition::parseMultiCompoundExpr(RegExprState origState)
{

    std::cout << "parseCompoundExpr" << std::endl;
    origState.printStrInfo();
    auto repeatResults = parseMultRepeatExpr(origState);
    if (repeatResults.size())
    {
        return repeatResults;
    }

    return parseMultSimpleExpr(origState);
}

std::set<TopDownRegExprRecognition::RegExprState> TopDownRegExprRecognition::parseMultRepeatExpr(RegExprState origState)
{

    std::cout << "parseRepeatExpr" << std::endl;
    origState.printStrInfo();
    auto afterStarIndex {origState.peekRegExprStar()};
    if (afterStarIndex == std::nullopt) return {};
    std::set<RegExprState> resultStates {{origState.curRegExpr, origState.curStr, afterStarIndex.value(), origState.curStrIndex}};
    std::set<RegExprState> inputStates {origState};
    while(inputStates.size())
    {
        std::set<RegExprState> outputStates{}, inputStatesBackup{};
        for(auto inputState : inputStates)
        {
            outputStates = parseMultSimpleExpr(inputState);
            for(auto outputState : outputStates)
            {
                outputState.printStrInfo();

                
                if (outputState.curStrIndex == inputState.curStrIndex)
                    continue;
                resultStates.insert({outputState.curRegExpr, outputState.curStr, afterStarIndex.value(), outputState.curStrIndex});
                outputState.setCurRegExprIndex(inputState.curRegExprIndex);  
                inputStatesBackup.insert(outputState);
            }
        }
        inputStates = inputStatesBackup;
    }
    std::cout << resultStates.size() << std::endl;
    return resultStates;
}


std::set<TopDownRegExprRecognition::RegExprState> TopDownRegExprRecognition::parseMultSimpleExpr(RegExprState origState)
{
    std::cout << "parseSimpleExpr" << std::endl;
    origState.printStrInfo();

    auto inputState {origState};
    if (inputState.eatInputToken())
    {
        return {inputState};
    }

    auto inParenEleSize = inputState.peekRegExprCloseParen();
    if (inParenEleSize.has_value())
    {
        inputState.curRegExpr = inputState.curRegExpr.substr(inputState.curRegExprIndex + 1, inParenEleSize.value());
        inputState.curRegExprIndex = 0;
        std::set<RegExprState> outputStates {};
        for(auto outputState : parseAlternativesExpr(inputState))
        {
            
            outputState.curRegExpr = origState.curRegExpr;
            outputState.curRegExprIndex += origState.curRegExprIndex + 2;
            outputStates.insert(outputState);
            
        }
        if (outputStates.size())
        {
            return outputStates;
        }
    }
    return {origState};
}