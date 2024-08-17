/* File: CLIProgressBar.h
 * Author: hfcloud <sxysxygm@gmail.com / 1441157749@qq.com> (also name: sxysxy)
 * Date: 2024-08-17
 * License: The MIT License only for this file. Always reserve this message when using this CLIProgressBar.py. 
 * LICENSE file is not required to attach with this file when embedded in other projects.
*/

#include <cstddef>
#include <cstdio>
#include <ios>
#include <memory>
#include <ostream>
#include <string>
#include <iostream>
#include <sys/_types/_timeval.h>
#include <sys/select.h>
#include <type_traits>
#include <utility>
#include <iomanip>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/time.h>
#endif

template<class Iterator, bool useCPPStream=true>
class CLIProgressBar {

    Iterator _begin, _end;
    std::string _desc, _unit;
    size_t _len;
    int _pstep, _pnum;
    std::ostream *_ost;
    double _lastTime;

    // for non-random-accessible iterations, we can't calculate current index by - operator,
    // so we use _cnt to record the number of processed items.
    size_t _cnt; 

#ifdef _WIN32
    HANDLE _hStdOut;
#endif
    static inline int get_terminal_width() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int columns;
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
            columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        } else {
            columns = 80; 
        }
        return columns;
#else
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
            return 80; // Default width if unable to get info
        } else {
            return w.ws_col;
        }
#endif
    }
#ifdef _WIN32
    void cliMoveArrowUp() {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if(GetConsoleScreenBufferInfo(_hStdOut, &csbi)) {
            csbi.dwCursorPosition.X = 0;
            if(csbi.dwCursorPosition.Y > 0) {
                csbi.dwCursorPosition.Y -= 1;
            }
            SetConsoleCursorPosition(_hStdOut, csbi.dwCursorPosition);
        }
    }
#else 
    void _out(const char * s) {
        if(this->_ost) (*this->_ost) << s;
        else printf("%s", s);
    }
    void _out(float v) {
        if(this->_ost) (*this->_ost) << v;
        else printf("%.2f", v);
    }
    void _out(double v) {
        if(this->_ost) (*this->_ost) << v;
        else printf("%.2lf", v);
    }
    void _out(int v) {
        if(this->_ost) (*this->_ost) << v;
        else printf("%d", v);
    }
    void _out(unsigned long long v) {
        if(this->_ost) (*this->_ost) << v;
        else printf("%llu", v);
    }
   
    void cliMoveArrowUp() {
        _out("\33[1A");
    }
#endif
    double getTime() {
#ifdef _WIN32
        FILETIME s;
        GetSystemTimePreciseAsFileTime(&s);
        ULARGE_INTEGER l;
        l.LowPart = s.dwLowDateTime;
        l.HighPart = s.dwHighDateTime;
        return l.QuadPart / 1e7;
#else 
        struct timeval s;
        gettimeofday(&s, nullptr);
        return s.tv_sec + s.tv_usec / 1e6;
#endif
    }

public:

    CLIProgressBar(Iterator begin, Iterator end, const std::string &desc = "", const std::string &unit = "it") {
        this->_begin = begin;
        this->_end = end;
        this->_desc = desc;
        this->_unit = unit;
        this->_cnt = 0;
        this->_ost = nullptr;
#ifdef _WIN32
        this->_hStdOut = GetStdHandle(-11)
#endif
        int cli_width = get_terminal_width();
        if (cli_width < 90) {
            _pstep = 5;
            _pnum = 20;
        } else {
            _pstep = 2;
            _pnum = 50;
        }
        _lastTime = getTime();
    }

    CLIProgressBar(Iterator begin, Iterator end, std::ostream &os, const std::string &desc = "", const std::string &unit = "it") 
        : CLIProgressBar<Iterator>(begin, end, desc, unit){
        this->_ost = std::addressof(os);
    }

    size_t length() const {
        return _end - _begin;
    }

    template<class Fn>
    void run(const Fn &f) {
        if(this->_ost) (*this->_ost) << std::fixed << std::setprecision(2);
        for(auto _cur = _begin; _cur != _end; ++_cur) {
            if(_cnt > 0) {
                cliMoveArrowUp();
            }
            float prog = 100.0f * (_cnt + 1) / length();
            _out(_desc.c_str());
            _out("[");
            int pcnt = 0;
            for(int i = _pstep; i <= prog+1; i += _pstep) {
                _out("=");
                pcnt += 1;
            }
            _out("*");
            for(int i = 0; i <= _pnum - pcnt - 1; i++) {
                _out(" ");
            }
            double now = getTime();
            double dt = now - _lastTime;
            double rate = 0;
           // _out(dt);
            if (fabs(dt) > 1e-8) {
                rate = 1.0 / dt;
            }
            _out("] ");
            _out((unsigned long long)(_cnt + 1));
            _out("/");
            _out((unsigned long long)(length()));
            _out(" ");
            if(dt > 0) {
                _out(rate);
            } else {
                _out("inf ");
            }
            _out(_unit.c_str());
            _out("/s");
            _out(" (");
            _out(prog);
            if(this->_ost) {
                _out(")%\n");
            } else {
                _out(")%%\n");
            } 
            if(fabs(dt) > 1e-8) {
                _lastTime = now;
            }
            _cnt++;
            f(*_cur);
        }
    }
};