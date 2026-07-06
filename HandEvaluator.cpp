#include"HandEvaluator.h"
using namespace std;

vector<Card> HandEvaluator::getTotalHand(Table t,Hand h)
{
    vector<Card>onTable = t.getCards();
    vector<Card>inHand = h.getCards();
    inHand.insert(inHand.begin(),onTable.begin(),onTable.end());
    return inHand;
}

void HandEvaluator::setTotalHand(Table t,Hand h)
{
    totalHand = getTotalHand(t,h);
}

map<int,int> HandEvaluator::getRankFreq()
{
    map<int,int> freq;
    for (int i = 0; i <= 14; i++) freq[i] = 0;
    for (auto& c : totalHand) freq[c.getRank()]++;
    return freq;
}

void HandEvaluator::setRankFreq()  { rankFreq = getRankFreq(); }

map<int,int> HandEvaluator::getSuitFreq()
{
    map<int,int> freq;
    for (int i = 0; i < 4; i++) freq[i] = 0;
    for (auto& c : totalHand) freq[c.getSuit()]++;
    return freq;
}

void HandEvaluator::setSuitFreq()  { suitFreq = getSuitFreq(); }

bool HandEvaluator::isFlush()
{
    for (int i = 0; i < 4; i++) if (suitFreq[i] >= 5) return true;
    return false;
}

bool HandEvaluator::isStraight()
{
    for (int i = 14; i >= 6; i--)
        if (rankFreq[i]>0&&rankFreq[i-1]>0&&rankFreq[i-2]>0&&rankFreq[i-3]>0&&rankFreq[i-4]>0)
            return true;
    // wheel: A-2-3-4-5
    if (rankFreq[14]>0&&rankFreq[5]>0&&rankFreq[4]>0&&rankFreq[3]>0&&rankFreq[2]>0)
        return true;
    return false;
}

bool HandEvaluator::isStraightFlush()
{
    if (!isFlush()) return false;
    int fs = -1;
    for (int i = 0; i < 4; i++) if (suitFreq[i]>=5) fs=i;
    map<int,int> fr;
    for (int i = 0; i <= 14; i++) fr[i] = 0;
    for (auto& c : totalHand) if (c.getSuit()==fs) fr[c.getRank()]++;
    for (int i = 14; i >= 6; i--)
        if (fr[i]>0&&fr[i-1]>0&&fr[i-2]>0&&fr[i-3]>0&&fr[i-4]>0) return true;
    if (fr[14]>0&&fr[5]>0&&fr[4]>0&&fr[3]>0&&fr[2]>0) return true;
    return false;
}

bool HandEvaluator::isRoyalFlush()
{
    if (!isStraightFlush()) return false;
    int fs = -1;
    for (int i = 0; i < 4; i++) if (suitFreq[i]>=5) fs=i;
    map<int,int> fr;
    for (int i = 0; i <= 14; i++) fr[i] = 0;
    for (auto& c : totalHand) if (c.getSuit()==fs) fr[c.getRank()]++;
    return fr[14]>0&&fr[13]>0&&fr[12]>0&&fr[11]>0&&fr[10]>0;
}

bool HandEvaluator::isFourOfAKind()
{
    for (int i = 2; i <= 14; i++) if (rankFreq[i]==4) return true;
    return false;
}

bool HandEvaluator::isThreeOfAKind()
{
    for (int i = 2; i <= 14; i++) if (rankFreq[i]==3) return true;
    return false;
}

bool HandEvaluator::isFullHouse()
{
    if (!isThreeOfAKind()) return false;
    int three = -1;
    for (int i = 14; i >= 2; i--) if (rankFreq[i]==3) { three=i; break; }
    for (int i = 14; i >= 2; i--) if (i!=three&&rankFreq[i]>=2) return true;
    return false;
}

bool HandEvaluator::isTwoPair()
{
    int first = -1;
    for (int i = 14; i >= 2; i--)
        if (rankFreq[i]>=2) { if (first==-1) first=i; else return true; }
    return false;
}

bool HandEvaluator::isOnePair()
{
    for (int i = 14; i >= 2; i--) if (rankFreq[i]>=2) return true;
    return false;
}

void HandEvaluator::setHighestRank()
{
    int v = -1;
    for (int i = 14; i >= 2; i--) if (rankFreq[i]>=1) { v=i; break; }
    for (auto& c : totalHand) if (c.getRank()==v) { highestRank=c; break; }
}

long long HandEvaluator::evaluate(const Table &t, const Hand &h)
{
    setTotalHand(t, h);
    setRankFreq();
    setSuitFreq();

    // encode: handRank * 15^5 + r1*15^4 + r2*15^3 + r3*15^2 + r4*15 + r5
    // gap between hand ranks = 15^5 = 759375, max tiebreaker = 14*50625+...+14 = 759374 → no overlap
    const long long B5 = 759375LL, B4 = 50625LL, B3 = 3375LL, B2 = 225LL, B1 = 15LL;

    auto encode = [&](int hr, vector<int> tb) -> long long {
        while ((int)tb.size() < 5) tb.push_back(0);
        return (long long)hr*B5 + tb[0]*B4 + tb[1]*B3 + tb[2]*B2 + tb[3]*B1 + tb[4];
    };

    auto getFlushSuit = [&]() {
        for (int i = 0; i < 4; i++) if (suitFreq[i]>=5) return i;
        return -1;
    };

    auto getFlushRankFreq = [&](int fs) {
        map<int,int> fr;
        for (int i = 0; i <= 14; i++) fr[i] = 0;
        for (auto& c : totalHand) if (c.getSuit()==fs) fr[c.getRank()]++;
        return fr;
    };

    if (isRoyalFlush()) return encode(10, {14,13,12,11,10});

    if (isStraightFlush()) {
        int fs = getFlushSuit();
        auto fr = getFlushRankFreq(fs);
        if (fr[14]>0&&fr[5]>0&&fr[4]>0&&fr[3]>0&&fr[2]>0) return encode(9, {5,4,3,2,1});
        for (int i = 14; i >= 6; i--)
            if (fr[i]>0&&fr[i-1]>0&&fr[i-2]>0&&fr[i-3]>0&&fr[i-4]>0)
                return encode(9, {i,i-1,i-2,i-3,i-4});
    }

    if (isFourOfAKind()) {
        int quad = -1;
        for (int i = 14; i >= 2; i--) if (rankFreq[i]==4) { quad=i; break; }
        int kicker = -1;
        for (int i = 14; i >= 2; i--) if (i!=quad&&rankFreq[i]>0) { kicker=i; break; }
        return encode(8, {quad, kicker});
    }

    if (isFullHouse()) {
        int three=-1, pair=-1;
        for (int i = 14; i >= 2; i--) if (rankFreq[i]==3) { three=i; break; }
        for (int i = 14; i >= 2; i--) if (i!=three&&rankFreq[i]>=2) { pair=i; break; }
        return encode(7, {three, pair});
    }

    if (isFlush()) {
        int fs = getFlushSuit();
        vector<int> fc;
        for (auto& c : totalHand) if (c.getSuit()==fs) fc.push_back(c.getRank());
        sort(fc.rbegin(), fc.rend());
        fc.resize(5);
        return encode(6, fc);
    }

    if (isStraight()) {
        if (rankFreq[14]>0&&rankFreq[5]>0&&rankFreq[4]>0&&rankFreq[3]>0&&rankFreq[2]>0)
            return encode(5, {5,4,3,2,1});
        for (int i = 14; i >= 6; i--)
            if (rankFreq[i]>0&&rankFreq[i-1]>0&&rankFreq[i-2]>0&&rankFreq[i-3]>0&&rankFreq[i-4]>0)
                return encode(5, {i,i-1,i-2,i-3,i-4});
    }

    if (isThreeOfAKind()) {
        int three=-1;
        for (int i = 14; i >= 2; i--) if (rankFreq[i]==3) { three=i; break; }
        vector<int> tb = {three};
        for (int i = 14; i >= 2; i--) if (i!=three&&rankFreq[i]>0) { tb.push_back(i); if((int)tb.size()==3) break; }
        return encode(4, tb);
    }

    if (isTwoPair()) {
        int p1=-1, p2=-1;
        for (int i = 14; i >= 2; i--) if (rankFreq[i]>=2) { if(p1==-1)p1=i; else{p2=i;break;} }
        int kicker=-1;
        for (int i = 14; i >= 2; i--) if (i!=p1&&i!=p2&&rankFreq[i]>0) { kicker=i; break; }
        return encode(3, {p1, p2, kicker});
    }

    if (isOnePair()) {
        int pair=-1;
        for (int i = 14; i >= 2; i--) if (rankFreq[i]>=2) { pair=i; break; }
        vector<int> tb = {pair};
        for (int i = 14; i >= 2; i--) if (i!=pair&&rankFreq[i]>0) { tb.push_back(i); if((int)tb.size()==4) break; }
        return encode(2, tb);
    }

    // High card: top 5
    vector<int> tb;
    for (int i = 14; i >= 2; i--) if (rankFreq[i]>0) { tb.push_back(i); if((int)tb.size()==5) break; }
    return encode(1, tb);
}

void HandEvaluator::setHeat(const Table &t, const Hand &h)
{
    heat = evaluate(t, h);
}
