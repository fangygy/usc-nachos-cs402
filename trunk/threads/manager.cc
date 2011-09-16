// Manager
#include "movietheater.h"

void Manager::work() {
    while (true) {
        randToBreak(lBuyTickets, (Employee**)tc, MAX_TC);
        randToBreak(lBuyFood, (Employee**)cc, MAX_CC);
        randToBreak(lCheckTickets, (Employee**)tt, MAX_TT);
    }
}

// rand ask employee to break
void Manager::randToBreak(Lock * lockWaiting, Employee ** clerk, int count) {
    // lock to get in line
    lockWaiting->Acquire();
    for (int i = 0;i < count; ++i) {
        if (!clerk[i]->getWaitingSize()) {
            // rand to break
            if (rand()%5 == 0) {
                clerk[i]->lock->Acquire();
                clerk[i]->setIsBreak(true);
                clerk[i]->lock->Release();
            }
        } else if (clerk[i]->getWaitingSize() < 3) {
            int j = 0;
            for (j = 0;j < count; ++i) {
                if (i == j) continue;
                if (!clerk[j]->getIsBreak()) {
                    break;
                }
            }
            if (j != count && rand()%5 == 0) {
                clerk[i]->lock->Acquire();
                clerk[i]->setIsBreak(true);
                // if not busy, manager should signal clerk to get break
                if (!clerk[i]->getIsBusy()) {
                    clerk[i]->condition[1]->Signal(clerk[i]->lock);
                }
                clerk[i]->lock->Release();
            }
        }
    }
    lockWaiting->Release();

}
Manager::Manager(int mrId) : Employee(mrId, "Manager"){
}

