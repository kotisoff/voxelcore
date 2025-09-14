local data = "hello, world"
local complete = false

network.tcp_open(7645, function (client)
    start_coroutine(function()
        local received = client:recv(1024)
        if received then
            assert (data, utf8.tostring(received))
            complete = true
        end
        coroutine.yield()
    end, "client-listener")
end)

network.tcp_connect("localhost", 7645, function (socket)
    socket:send(data)
end)

app.sleep_until(function () return complete end)
