
class Timer{
    private:
        int hours;
        int minutes;
        int seconds;

    public:
        Timer();
        void setTime(int hours, int minutes, int seconds);
        void setHours(int hours){ this->hours = hours; }
        void setMinutes(int minutes){ this->minutes = minutes; }
        void setSeconds(int seconds){ this->seconds = seconds; }

        int getHours(){ return this->hours; }
        int getMinutes(){ return this->minutes; }
        int getSeconds(){ return this->seconds; }

        void incrementTimer();
};
