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
        //
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

    // Rebuilds the link chain so every element points to the next one in
    // insertion order, and resets traversal state so all elements are
    // considered "uncompleted" again.
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

    // Restarts traversal from the first uncompleted element without
    // touching the completion state itself.
    void pointToFirstUncompleted()
    {
        previous = NONE;
        current = NONE;
        next = firstUncompleted;
    }

    // Returns a pointer to the next uncompleted element, or nullptr if
    // there are none left. Advances previous/current/next bookkeeping
    // needed for an O(1) removal in markMostRecentAsCompleted().
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

    // Splices the most recently returned element out of the uncompleted
    // chain in O(1). No-op if there is no current element (e.g. called
    // before getNextUncompleted() or twice in a row without an
    // intervening getNextUncompleted()).
    void markMostRecentAsCompleted()
    {
        if (current == NONE)
        {
            return;
        }

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
