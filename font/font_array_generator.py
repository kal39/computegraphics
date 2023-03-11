from PIL import Image

char_size = (6, 10)
img_size = (96, 60)
image_size_chars = (16, 6)

data = [0 if v == 0 else 1 for v in Image.open("font/font_64.png").convert("L").getdata()]

for i in range(image_size_chars[1]):
    for j in range(image_size_chars[0]):
        num1 = 0x00000000
        num2 = 0x00000000
        string = ""
        for ii in  range(char_size[1]):
            for jj in range(char_size[0]):
                v = data[(i * char_size[1] + ii) * img_size[0] + (j * char_size[0] + jj)]
                
                string += str(v)
                if ii < 5: num1 = (num1 << 1 | v)
                else: num2 = (num2 << 1 | v)

        # char = chr((i * image_size_chars[0] + j) + 32)
        # print("0x{:08X}, 0x{:08X}, // '{:}'".format(num1, num2, char))
        print("0x{:08X}, 0x{:08X}, ".format(num1, num2))