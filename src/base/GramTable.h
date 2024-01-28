#pragma once
#include <map>
#include <string>
#include <string_view>
#include <tuple>
#include <set>
#include <vector>
#include <iostream>
#include <optional>
#include <ranges>
template<std::bidirectional_iterator _InputIterator, typename _Function> 
void
CartesianProduct(_InputIterator __first, _InputIterator __last, _InputIterator __first2,
					 _InputIterator __last2,_Function __f)
{
    if (__first == __last)
    {
        for (auto __it{__first2}; __it != __last2; ++__it)
		{
			__f("", *__it);
		}	
    }
    else
    {
        for (; __first != __last; ++__first)
        {
            for (auto __it{__first2}; __it != __last2; ++__it)
            {
                __f(*__first, *__it);
            }	
        }	
    }
	return; 
}
class GramTable
{
    public:
        using ProdLeft = std::string;
        using ProdStr = std::string;
        using ProdRightAlt = std::vector<std::string>;
        using ProdRightAlts = std::vector<ProdRightAlt>;
        using MinMap = std::map<std::string, int>;
        using FirstMap = std::map<std::string, std::set<std::string>>;
        using FirstKMap = FirstMap;
        using FollowKMap = FirstMap;
        using GramMap = std::map<ProdLeft, ProdRightAlts>;
        using PartitionTableItem = std::vector<std::string_view>;
        using PartitionTable = std::vector<PartitionTableItem>;

        using PartitionOptionTuple = std::tuple<std::optional<GramTable::MinMap>, std::optional<GramTable::FirstMap>>;
        using PartitionOption = std::optional<std::tuple<ProdRightAlt, PartitionOptionTuple>>;
        
        using BnfNoTermToTermMap = std::map<std::string_view, std::set<std::string_view>>;
        using BnfTwoTermToTermMap = std::map<std::tuple<std::string_view, std::string_view>, std::set<std::string_view>>;
        using ProdsPair = std::vector<std::tuple<std::string, std::string>>;
        std::optional<std::tuple<GramTable::BnfNoTermToTermMap, GramTable::BnfTwoTermToTermMap>> getBNFInfo();
    public:
        GramTable(const std::vector<std::string> gramLines);
        GramMap& getGramMap()
        {
            return grammerMap;
        }
        std::string getStartNoTerm()
        {
            return startNoTerm;
        }
        void print() const;
        static bool isTerm(std::string_view str);
        MinMap getMinMap(int defaultMaxSymbol = 99999);
        FirstMap getFirstMap();
        std::optional<FirstKMap> getFirstKMap(size_t k);
        std::optional<FollowKMap> getFollowKMap(size_t k, FirstMap &firstKMap);
        static PartitionTable partition(std::string_view str, int num, const PartitionOption &option);
        static inline std::string firstCharToString(std::string_view input)
        {
            return std::string(input.empty() ? nullSymbol : input.substr(0, 1));
        }
        std::optional<std::map<ProdLeft, ProdRightAlt>> getProdsWithOnlyTerms();

        static bool isEmptyProd(const ProdRightAlt &rhs)
        {
            return rhs.size() == 1 && rhs[0].size() == 0;
        }
        const ProdsPair& getProdsPair()
        {
            return prodsPairs;
        }
        // auto getPairs()
        // {
        //     return grammerMap | std::views::transform([](auto &cur){ 
        //         return std::views::cartesian_product(std::views::single(x.first), x.second);
        //     }) | std::views::join;
        // }
        inline static const std::string inputEndTerm {"#"};
        inline static const std::string nullSymbol {""};
        static const inline std::string epsilon{"\u03B5"}; //use for print nullSymbol
        inline static const std::string startNoTerm {"S"};
    private:
        static void calcPartition(const std::string_view &str, int num, PartitionTable &res,
        PartitionTableItem &resItem, const std::vector<int> &minVec, const std::vector<std::set<std::string>> &firstVec);
        void parseGramLine(std::string_view line);
        bool calcMinMap(MinMap &minMap);
        bool calcFirstMap(FirstMap &firstMap);
        bool calcFirstKMap(FirstKMap &firstKMap, size_t k);
        bool calcFollowKMap(FollowKMap &followKMap, size_t k, FirstKMap &firstKMap);
        GramMap grammerMap;
        std::set<std::string_view> noTermSet;
        std::set<std::string_view> termSet;
        
        ProdsPair prodsPairs;  
        
        

};
