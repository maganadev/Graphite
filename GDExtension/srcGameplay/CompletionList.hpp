#ifndef CompletionList_hpp
#define CompletionList_hpp
#include <cstdint>
#include <vector>

template <class T>
class CompletionList
{
private:
    static constexpr uint32_t NONE = UINT32_MAX;
    std::vector<T> elements;
    std::vector<uint32_t> indexPointingTo;
    uint32_t previous;
    uint32_t current;
    uint32_t next;
    uint32_t firstUncompleted;

public:
    CompletionList() : previous(NONE), current(NONE), next(NONE), firstUncompleted(NONE)
    {
    }

    void push_back(T element)
    {
        elements.push_back(element);
        indexPointingTo.push_back(NONE);
    }

    void pop_back()
    {
        elements.pop_back();
        indexPointingTo.pop_back();
    }

    T back()
    {
        return elements.back();
    }

    void setAt(size_t index, T element)
    {
        elements[index] = element;
    }

    T getAt(size_t index)
    {
        return elements[index];
    }

    size_t size()
    {
        return elements.size();
    }

    void resetCompletionStates()
    {
        for (size_t i = 0; i < elements.size(); i++)
        {
            indexPointingTo[i] = (i + 1 < elements.size()) ? static_cast<uint32_t>(i + 1) : NONE;
        }
        previous = NONE;
        current = NONE;
        next = elements.empty() ? NONE : 0;
        firstUncompleted = next;
    }

    void pointToFirstUncompleted()
    {
        previous = NONE;
        current = NONE;
        next = firstUncompleted;
    }

    T* getNextUncompleted()
    {
        if (next == NONE)
        {
            return nullptr;
        }
        T* toReturn = &(elements[next]);
        uint32_t oldNext = next;
        uint32_t oldCurrent = current;
        uint32_t oldPrevious = previous;
        next = indexPointingTo[oldNext];
        current = oldNext;
        if (oldPrevious != NONE)
        {
            if (indexPointingTo[oldPrevious] != current)
            {
                previous = indexPointingTo[oldPrevious];
            }
        }
        else
        {
            previous = oldCurrent;
        }
        return toReturn;
    }

    void markMostRecentAsCompleted()
    {
        if (current == NONE)
            return;
        if (previous != NONE)
        {
            indexPointingTo[previous] = next;
        }
        else
        {
            firstUncompleted = next;
        }
        current = NONE;
        previous = NONE;
    }
};
#endif
