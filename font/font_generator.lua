BmpFile = {}

function BmpFile:new(path)
    local bytes = Bytes:new(io.open(path, "rb"):read("a"))
    -- file header
    if bytes:readBytes(2) ~= 0x4D42 then error("not a bmp file") end
    self.fileSize = bytes:readBytes(4)
    if bytes:readBytes(4) ~= 0 then error("corrupted file") end
    local off = bytes:readBytes(4)
    -- info header
    bytes:readBytes(4) -- ignore
    self.w = bytes:readBytes(4)
    self.h = bytes:readBytes(4)
    if bytes:readBytes(2) ~= 1 then error("corrupted file") end
    self.channels = math.floor(bytes:readBytes(2) / 8)
    local comp = bytes:readBytes(4)
    if comp ~= 0 and comp ~= 3 then error("compression not supported") end

    self.data = table.move(bytes.data, off + 1, off + self.w * self.h * 4, 1)
    return self
end

function BmpFile:at(x, y)
    return {
        r = self.data[((self.h - y - 1) * self.w + x) * 4 + 1],
        g = self.data[((self.h - y - 1) * self.w + x) * 4 + 2],
        b = self.data[((self.h - y - 1) * self.w + x) * 4 + 3],
        a = self.data[((self.h - y - 1) * self.w + x) * 4 + 4],
    }
end

Bytes = {}

function Bytes:new(data)
    self.next = 1
    self.data = {}
    for i = 1, data:len() do self.data[i] = data:sub(i, i):byte() end
    return self
end

function Bytes:readBytes(count)
    local bytes = 0
    for i = 1, count do bytes = bytes | (self:readByte() << (i - 1) * 8) end
    return bytes
end

function Bytes:readByte()
    self.next = self.next + 1
    return self.data[self.next - 1]
end

local img = BmpFile:new("font_64.bmp")

for i = 0, 6 - 1 do
    for j = 0, 16 - 1 do
        local num1, num2 = 0, 0
        for ii = 0, 10 - 1 do
            for jj = 0, 6 - 1 do
                local v = math.floor(img:at(j * 6 + jj, i * 10 + ii).r / 255)
                num1 = (ii < 5 and (num1 << 1 | v) or num1)
                num2 = (ii > 4 and (num2 << 1 | v) or num2)
            end
        end
        print(string.format("0x%08X, 0x%08X,", num1, num2))
    end
end
