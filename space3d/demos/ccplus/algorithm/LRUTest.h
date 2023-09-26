#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>

namespace algorithm::lruTest
{
namespace lruTest_01
{
    // thanks: https://www.geeksforgeeks.org/program-for-least-recently-used-lru-page-replacement-algorithm/
    /*
        Let capacity be the number of pages that
        memory can hold.  Let set be the current
        set of pages in memory.
        
        1- Start traversing the pages.
         i) If set holds less pages than capacity.
           a) Insert page into the set one by one until 
              the size  of set reaches capacity or all
              page requests are processed.
           b) Simultaneously maintain the recent occurred
              index of each page in a map called indexes.
           c) Increment page fault
         ii) Else 
           If current page is present in set, do nothing.
           Else 
             a) Find the page in the set that was least 
             recently used. We find it using index array.
             We basically need to replace the page with
             minimum index.
             b) Replace the found page with current page.
             c) Increment page faults.
             d) Update index of current page.
        
        2. Return page faults.
    */
// Function to find page faults using indexes (Least Recently Used (LRU) )
int pageFaults(int pages[], int n, int capacity)
{
    // To represent set of current pages. We use
    // an unordered_set so that we quickly check
    // if a page is present in set or not
    std::unordered_set<int> s;

    // To store least recently used indexes
    // of pages.
    std::unordered_map<int, int> indexes;

    // Start from initial page
    int page_faults = 0;
    for (int i = 0; i < n; i++)
    {
        // Check if the set can hold more pages
        if (s.size() < capacity)
        {
            // Insert it into set if not present
            // already which represents page fault
            if (s.find(pages[i]) == s.end())
            {
                s.insert(pages[i]);

                // increment page fault
                page_faults++;
            }

            // Store the recently used index of
            // each page
            indexes[pages[i]] = i;
        }

        // If the set is full then need to perform lru
        // i.e. remove the least recently used page
        // and insert the current page
        else
        {
            // Check if current page is not already
            // present in the set
            if (s.find(pages[i]) == s.end())
            {
                // Find the least recently used pages
                // that is present in the set
                int lru = INT_MAX, val;
                for (auto it = s.begin(); it != s.end(); it++)
                {
                    if (indexes[*it] < lru)
                    {
                        lru = indexes[*it];
                        val = *it;
                    }
                }

                // Remove the indexes page
                s.erase(val);

                // insert the current page
                s.insert(pages[i]);

                // Increment page faults
                page_faults++;
            }

            // Update the current page index
            indexes[pages[i]] = i;
        }
    }

    return page_faults;
}

// Driver code
void testMain()
{
    int pages[]  = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
    int n        = sizeof(pages) / sizeof(pages[0]);
    int capacity = 4;
    std::cout << pageFaults(pages, n, capacity);
}
}
void testMain()
{
    std::cout << "\nalgorithm::lruTest::testMain() begin.\n";
    lruTest_01::testMain();
    std::cout << "\nalgorithm::lruTest::testMain() end.\n";
}
}