local Schedule = {
    __index = {
        set_interval = function(self, ms, callback)
            local id = self._next_interval
            self._intervals[id] = {
                last_called = 0.0,
                delay = ms / 1000.0,
                callback = callback,
            }
            self._next_interval = id + 1
            return id
        end,
        tick = function(self, dt)
            local timer = self._timer + dt
            for id, interval in pairs(self._intervals) do
                if timer - interval.last_called >= interval.delay then
                    xpcall(interval.callback, function(s)
                        debug.error(s..'\n'..debug.traceback())
                    end)
                    interval.last_called = timer
                end
            end
            self._timer = timer
        end,
        remove_interval = function (self, id)
           self._intervals[id] = nil
        end
    }
}

return function ()
    return setmetatable({
        _next_interval = 1,
        _timer = 0.0,
        _intervals = {},
    }, Schedule)
end
