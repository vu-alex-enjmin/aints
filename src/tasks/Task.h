#ifndef TASK_H_
#define TASK_H_

#include <vector>
#include <utility>

class TaskAgent;

class Task
{
    public:
        Task();
        ~Task();

        void SetAsCompleted();

        void AddCandidate(TaskAgent *candidate);
        void SelectCandidate();
        void ClearCandidates();
        void Unassign();
        bool IsAssigned() const;
        bool IsCompleted() const;
        int GetId() const;
        
        virtual void GiveOrderToAssignee() = 0;
        virtual bool IsValid() = 0;

    protected:
        unsigned int _id;
        bool _completed;
        TaskAgent *_assignee;
        std::vector<std::pair<TaskAgent*, int>> _candidateFitnessPairs;

        virtual int EvaluateCandidate(TaskAgent *candidate) = 0;

    private:
        static unsigned int _nextId;
};

#endif // TASK_H
