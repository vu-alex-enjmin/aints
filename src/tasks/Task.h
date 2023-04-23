#ifndef TASK_H_
#define TASK_H_

class TaskBoard;
class TaskAgent;

class Task
{
    public:
        int Priority;
        Task(int priority = 0);
        void SetBoard(TaskBoard* board);
        void AssignTo(TaskAgent* agent);
        void FreeUp();
        void Cancel(); // Destroy task from indirect source
        void Complete(); // Destroy task from agent
        bool IsAssigned() const;
        int GetId() const;

    private:
        static unsigned int _nextId;
        const unsigned int _id;
        TaskBoard* _board;
        TaskAgent* _assignee;
};

#endif // TASK_H
