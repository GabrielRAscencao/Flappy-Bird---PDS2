#ifndef SCOREMANAGER_HPP
#define SCOREMANAGER_HPP

class ScoreManager {
private:
    int score;

public:
    ScoreManager();
    void reset();
    void increment();
    int getScore() const;
};

#endif
