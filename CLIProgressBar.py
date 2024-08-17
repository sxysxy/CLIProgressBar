# File: CLIProgressBar.py
# Author: hfcloud <sxysxygm@gmail.com / 1441157749@qq.com> (also name: sxysxy)
# Date: 2024-08-17
# License: The MIT License only for this file. Always reserve this message when using this CLIProgressBar.py. 
# LICENSE file is not required to attach with this file when embedded in other projects.

from typing import Iterable
import os
import time
import sys
import ctypes

if sys.platform in ['win32', 'cygwin']:
    from ctypes.wintypes import _COORD, WORD, SMALL_RECT
    
    class CONSOLE_SCREEN_BUFFER_INFO(ctypes.Structure):
         _fields_ = [("dwSize", _COORD), ("dwCursorPosition", _COORD), ("_", WORD), ("__", SMALL_RECT), ("___", _COORD)]
    
    def winMoveArrawUp(hStdOut):
        csbi = CONSOLE_SCREEN_BUFFER_INFO()
        ctypes.windll.kernel32.GetConsoleScreenBufferInfo(hStdOut, ctypes.byref(csbi))
        csbi.dwCursorPosition.X = 0
        if csbi.dwCursorPosition.Y > 0:
            csbi.dwCursorPosition.Y -= 1
        ctypes.windll.kernel32.SetConsoleCursorPosition(hStdOut, csbi.dwCursorPosition)

class _CLIProgressBar:
    def _cliMoveArrowUp(self):
        if self._is_windows:
            winMoveArrawUp(self._hStdOut)
        else:
            print("\33[1A", end='')
    
    def __init__(self, data : Iterable, desc : str, unit : str) -> None:
        self.data = data
        self.desc = desc 
        self.unit = unit
        if hasattr(self.data, "__len__"):
            self._len = len(self.data)
        else:
            self._len = 0
        self._iter = self.data.__iter__()
        self._cnt = 0
        self._lastTime = time.time()
        self._cli_width = os.get_terminal_size().columns
        
        if self._cli_width < 90:
            self._pstep = 5
            self._pnum = 20
        else:
            self._pstep = 2
            self._pnum = 50
            
        if sys.platform in ['win32', 'cygwin']:
            self._hStdOut = ctypes.windll.kernel32.GetStdHandle(ctypes.wintypes.DWORD(-11))
            self._is_windows = True
        else:
            self._hStdOut = None
            self._is_windows = False
        
    def __len__(self):
        return self._len

class CLIProgressBar(_CLIProgressBar):
    def __init__(self, data: Iterable, desc : str = "", unit : str = "it") -> None:
        super().__init__(data, desc, unit)
    
    def __iter__(self):
        for x in self._iter:
            if self._len > 0:
                if self._cnt > 0:
                    self._cliMoveArrowUp()
                prog = 100.0 * (self._cnt+1) / self.__len__()
                print(f"{self.desc}[",end='')
                pcnt = 0
                for i in range(self._pstep, int(prog)+1, self._pstep):
                    print('=',end='')
                    pcnt += 1
                print('*',end='')
                for i in range(0, self._pnum-pcnt):
                    print(' ',end='')
                nt = time.time()
                dt = nt-self._lastTime
                if dt > 0:
                    rate = (1.0/dt)
                    print(f"] {self._cnt+1}/{self._len} {rate:.2f}{self.unit}/s ({prog:.2f}%)          ")
                    self._lastTime = nt
                else:
                    print(f"] {self._cnt+1}/{self._len} inf {self.unit}/s ({prog:.2f}%)          ")
            else:
                pass
            self._cnt += 1
            yield x
        
if __name__ == "__main__":
    '''
    Just for test, ignore.
    '''
    import argparse
    ps = argparse.ArgumentParser()
    ps.add_argument("--num", default=100, type=int)
    ps.add_argument("--desc", default="Desc")
    ps.add_argument("--unit", default='it')
    ps.add_argument("--step", default=1, type=int)
    ps.add_argument("--sleep-time", default=0.01, type=float)
    args = ps.parse_args()
    
    data = list(range(0, args.num, args.step))
    data2 = []
    for i in CLIProgressBar(data, desc=args.desc, unit=args.unit):
        time.sleep(args.sleep_time)
        data2.append(i)
    print(data2)
        
        
