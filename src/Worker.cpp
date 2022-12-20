#include <random>
#include <string>
#include <stdlib.h>
#include <time.h>

#include "Worker.h"


class Worker {       
    private:            
        float speed;  

    public:
        float getSpeed() {return speed;};
        virtual float getCost(VTime t)
        virtual string getType();
};


class NewbieWorker: public Worker {
    public:
        Newbie() {
            speed = 0.8; // temp
        }
        float getCost(VTime t) {
            return 1.0; //temp
        }
        string getType() {
            return "Newbie";
        }
}

class NormalWorker: public Worker {
    public:
        NormalWorker() {
            speed = 1.0; // temp
        }
        float getCost(VTime t) {
            return 2.0; //temp
        }
        string getType() {
            return "Normal";
        }
}

class ExpertWorker: public Worker {
    public:
        ExpertWorker() {
            speed = 1.2; // temp
        }
        float getCost(VTime t) {
            return 3.0; //temp
        }
        string getType() {
            return "Expert";
        }
}