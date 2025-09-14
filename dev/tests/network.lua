for i=1,3 do
    local text = ""
    local complete = false

    for j=1,100 do
        text = text .. math.random(0, 9)
    end

    local server = network.tcp_open(7645, function (client)
        start_coroutine(function()
            local received_text = ""
            while client:is_alive() do
                local received = client:recv(512)
                if received then
                    received_text = received_text .. utf8.tostring(received)
                end
                coroutine.yield()
            end
            assert (received_text == text)
            complete = true
        end, "client-listener")
    end)

    network.tcp_connect("localhost", 7645, function (socket)
        start_coroutine(function()
            local ptr = 1
            while ptr < #text do
                local n = math.random(1, 20)
                socket:send(string.sub(text, ptr, ptr + n - 1))
                ptr = ptr + n
            end
            socket:close()
        end, "data-sender")
    end)

    app.sleep_until(function () return complete end)
    server:close()
end
