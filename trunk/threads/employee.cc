// Employee
#include "movietheater.h"

void Employee::setIsBusy(bool busy) {
    isBusy = busy;
}
bool Employee::getIsBusy() {
    return isBusy;
}

void Employee::setIsBreak(bool isOff) {
    isBreak = isOff;
}
bool Employee::getIsBreak() {
    return isBreak;
}

char * Employee::getEmployeeType() {
    return employeeType;
}
int Employee::getId() {
    return id;
}
int Employee::getGroupId() {
    return groupId;
}
void Employee::setGroupId(int gId) {
    groupId = gId;
}

Employee::Employee(int eId, char * type) {
    id = eId;
    sprintf(employeeType, "%s", type);
    //strcpy(employeeType, type);
    isBusy = false;
    isBreak = false;
    waitingSize = 0;
    char str[30];
    sprintf(str, "lock_%s_%d", employeeType, id);
    lock = new Lock(str);
    sprintf(str, "cond_waitingLine_%s_%d", employeeType, id);
    condition[0] = new Condition(str);
    sprintf(str, "cond_action_%s_%d", employeeType, id);
    condition[1] = new Condition(str); 
    sprintf(str, "cond_break_%s_%d", employeeType, id);
    condition[2] = new Condition(str);

   // printf("employee construction\n", str);
}

Employee::~Employee() {}

int Employee::getWaitingSize() {
    return waitingSize;
}

void Employee::addWaitingSize() {
    ++waitingSize;
}

void Employee::subWaitingSize() {
    --waitingSize;
}

