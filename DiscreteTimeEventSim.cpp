// Mateo Escamilla

#include <iostream>
#include <random>
#include <ctime>
#include <list>
#include <iomanip>
#include <float.h>
#include <stdlib.h>

#define ARR 0
#define DEP 1

/*  The Event class allows us to store the process type, the
 *  process time, and the ID of the individual process. The type
 *  will be either 0 for arrival (ARR) or 1 for departure (DEP).
 *  Basic helper functions are included to get the process info.
 */
class Event {
    public:
        int type;
        float time;
        int pID;

        // Constructor method to initialize each Event
        Event(int type, float time, int pID) {
            this->type = type;
            this->time = time;
            this->pID = pID;
        }

        int getType() {
            return type;
        }

        float getTime() {
            return time;
        }

        int getPID() {
            return pID;
        }
};

// This sortByTime bool allows us to .sort() our Event list by time
static bool sortByTime(const Event &lhs, const Event &rhs) {
    return lhs.time < rhs.time;
}


/*  The Simulation class contains all the variables and methods
 *  needed to run the Simulation. We have a constructor that 
 *  initializes a lot of our values, and sets the user inputted 
 *  average arrival rate and average service time. These methods
 *  follow closely to the professor's slides 7_simul. Changes were
 *  made to account for the inclusion of std::list.
 */
class Simulation {
    private:
        static const int MAX_DEPARTURES = 50;

        int processesArrived;
        int processesDeparted;
        float currentTime;
        float lambda;
        float avgServTime;
        int processesInReadyQueue;
        bool serverIdle;

        float runningServTime;
        float finalDepartTime;

        std::list<Event> eventQueue;

    public:

        // Constructor method that initializes most of the variables
        Simulation(float avgArriRate, float avgServTime) {
            this->lambda = avgArriRate;
            this->avgServTime = avgServTime;
            processesArrived = 0;
            processesDeparted = 0;
            runningServTime = 0;
            finalDepartTime = 0;
        }

        // Initialization method that primes the simulation for run()
        void initialization() {
            currentTime = 0;
            serverIdle = true;
            processesInReadyQueue = 0;
            float t = randomInterArriv();
            schedule_event(ARR, t);
        }

        /*  The run() method is a looping functions that continues
         *  to loop until the system has reached a max number of departures.
         *  Each iteration, it grabs the first Event from the list, and it
         *  sets the current time to that Event's time. Then it checks to see
         *  what process state it is, and follows a switch statement. If the
         *  case is a departure, we need to check to see if this is the final
         *  departure, if so, record that departure time. Lastly, it pops the
         *  event (at the front) that was just handled.
         */
        void run() {
            while (processesDeparted <= MAX_DEPARTURES) {
                Event e = eventQueue.front();
                currentTime = e.getTime();
                switch (e.getType()) {
                    case ARR:
                        std::cout << "PID: " << e.getPID() << "  Type: Arrival    Time: " << e.getTime() << "\n";
                        arr_handler(); 
                        break;
                    case DEP:
                        if (e.getPID() == MAX_DEPARTURES) {
                            finalDepartTime = e.getTime();
                        }
                        std::cout << "PID: " << e.getPID() << "  Type: Departure  Time: " << e.getTime() << "\n";
                        dep_handler();
                        break;
                }
                eventQueue.pop_front();
            }
        }

        /*  Arrival handler handles all incoming process arrivals. It checks
         *  if the server is idle, and either generates a random service time
         *  for the Event's departure, or throws it into the queue. In either
         *  case, the next arrival is scheduled, following current time +
         *  a random interarrival time a.
         *  (running service time is a running total of all sim service times)
         */
        void arr_handler() {
            if (serverIdle == true) {
                serverIdle = false;
                float s = randomServTime();
                runningServTime += s;
                schedule_event(DEP, currentTime + s);
            } else {
                processesInReadyQueue++;
            }
            float a = randomInterArriv();
            float nextTime = (currentTime + a);
            schedule_event(ARR, nextTime);
        }

        /*  Departure handler handles all outgoing departures. It checks if
         *  the process ready queue is empty, and if so, sets the server back
         *  to idle. If not, it decrements the queue by 1, and generates a random
         *  service time which is used to schedule a future departure Event.
         *  (running service time is a running total of all sim service times)
         *  If our finaldeparturetime has been recorded, do not add the next
         *  service time to runningServTime. If we do, we will have a utilization
         *  above 100%.
         */
        void dep_handler() {
            if (processesInReadyQueue == 0) {
                serverIdle = true;
            } else {
                processesInReadyQueue--;
                float s = randomServTime();
                if (finalDepartTime == 0) {
                    runningServTime += s;
                }
                float nextTime = (currentTime + s);
                schedule_event(DEP, nextTime);
            }
        }

        /*  Schedule event first decides if the next event is an arrival, or a 
         *  departure, then, it either creates a new event of that state, and pushes
         *  it to the back of the list. The processesArrived and processesDeparted
         *  variables keep track of how many events have come or gone.
         */
        void schedule_event(int state, float time) {
            if (state == ARR) {
                processesArrived++;
                Event e = Event(state, time, processesArrived);
                eventQueue.push_back(e);
            } else {
                processesDeparted++;
                Event e = Event(state, time, processesDeparted);
                eventQueue.push_back(e);
            }
            eventQueue.sort(sortByTime);
        }

        /*  These next four methods are used to calculate the observed
         *  throughput, utilization, processes in ready queue, and
         *  average turnaround time.
         */
        float throughputSolver() {
            float tempMu = (finalDepartTime / processesDeparted);

            return (1.0 / tempMu);
        }

        float avgUtilSolver() {
            return (runningServTime / finalDepartTime);
        }

        float avgProcessInRQSolver() {
            float averageUtil = avgUtilSolver();
            float tempQ = (averageUtil / (1.0 - averageUtil));

            return (tempQ - averageUtil);
        }

        float avgTurnaroundSolver() {
            float tempUtil = avgUtilSolver();
            float tempThroughput = throughputSolver();

            float tempQ = (tempUtil / (1.0 - tempUtil));
            float tempLambda = (tempUtil * tempThroughput);

            return (tempQ / tempLambda);
        }

        /*  This method prints the metrics of the simulation, using 
         *  calculations gathered from the above functions.
         */
        void print_metrics() {
            float averageTurnaround = avgTurnaroundSolver();
            float totalThroughput = throughputSolver();
            float averageUtil = avgUtilSolver();
            float averageProcRQ = avgProcessInRQSolver();

            std::cout << std::setprecision(2);
            std::cout << "Performance Metrics with average arrival rate of " << lambda << " and average service time of " << avgServTime << ":\n";
            std::cout << std::fixed << std::setprecision(4);
            std::cout << "Average turnaround time of the processes was " << averageTurnaround << " seconds.\n";
            std::cout << "Total throughput was " << totalThroughput << " processes done per second.\n";
            std::cout << "Average CPU utilization was " << (averageUtil * 100) << "%.\n";
            std::cout << "Average number of processes in the Ready Queue was " << averageProcRQ << ".\n";
        }

        /*  These two functions generate our random inter arrival
         *  time and the random service time.
         */
        float randomInterArriv() {
            float z = (rand())/(RAND_MAX * 1.0);
            return (-1.0/lambda) * (logf(z));
        }

        float randomServTime() {
            float z = (rand())/(RAND_MAX * 1.0);
            return (-avgServTime * (logf(z)));
        }
};

/*  Main contains an argument checker, creates the Simulation
 *  object, which will initialize, run, and lastly print the metrics
 *  of the program.
 */
int main(int argc, char* argv[]) {
    srand(time(NULL));

    float userChoiceLambda = 0;
    float userChoiceService = 0;

    if (argc != 3) {
        std::cout << "Invalid amount of arguments...\n";
        std::cout << "Correct usage: ./prog2 <average_arrival_rate> <average_service_time>\n";
        return -1;
    }

    userChoiceLambda = atof(argv[1]);
    userChoiceService = atof(argv[2]);

    Simulation sim1 = Simulation(userChoiceLambda, userChoiceService);

    sim1.initialization();
    sim1.run();
    sim1.print_metrics();

    return 0;
}