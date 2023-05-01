#ifndef TASK_H_
#define TASK_H_

#include <vector>
#include <utility>

class TaskAgent;

// Piece of work that a TaskAgent can perform
class Task
{
    public:
        /*
        =========================================
            Methods
        =========================================
        */

        Task();
        // Unassigns the Task on destruction
        ~Task();
        // Sets the Task as performed and finished successfully
        void SetAsCompleted();
        // Adds candidate to vector of candidates after calculating its fitness
        void AddCandidate(TaskAgent *candidate);
        // Finds fittest candidate among current candidates and puts it as assignee
        // (Note that the fittest candidate has the highest fitness value)
        void SelectCandidate();
        // Removes all candidates from vector of candidates
        void ClearCandidates();
        // Removes task from assignee and remove assignee 
        void Unassign();
        // Checks if the Task has an assignee
        bool IsAssigned() const;
        // Checks if Task was previously SetAsCompleted
        bool IsCompleted() const;
        // Returns the Task's unique id
        int GetId() const;
        
        // Makes assignee do Task
        virtual void GiveOrderToAssignee() = 0;
        // Checks if a task is valid.
        // (a Task's validity is purely subjective from one Task to another)
        virtual bool IsValid() = 0;

    protected:
        /*
        =========================================
            Attributes
        =========================================
        */

        // Task's unique identifier
        unsigned int _id;
        // Whether the task was performed successfully
        bool _completed;
        // Chosen candidate for the Task
        // nullptr by default
        TaskAgent *_assignee;
        // Potential candidates to be assigned for the Task
        // <candidate, fitness>
        std::vector<std::pair<TaskAgent*, int>> _candidateFitnessPairs;

        /*
        =========================================
            Methods
        =========================================
        */

        // Evaluates a candidate's fitness for the Task
        // the higher the value, the better
        virtual int EvaluateCandidate(TaskAgent *candidate) = 0;

    private:
        /*
        =========================================
            Attributes
        =========================================
        */
        
        // ID to assign for the next instantiated Task instance
        static unsigned int _nextId;
};

#endif // TASK_H
