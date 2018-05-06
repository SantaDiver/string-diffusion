from PIL import Image, ImageDraw

def draw_rectangle(draw, coordinates, outline, fill, width=1):
    for i in range(width):
        rect_start = (coordinates[0][0] + i, coordinates[0][1] + i)
        rect_end = (coordinates[1][0] - i, coordinates[1][1] - i)
        if i==width-1:
            draw.rectangle((rect_start, rect_end), outline = outline, fill=fill)
        else:
            draw.rectangle((rect_start, rect_end), outline = outline)

def disbalance_start(i, j, width_len, height_len):
    if i<4:
        return None
    elif j<(i-4+1)/2 or width_len-j<(i-4)/2+1:
        if j<3:
            return 'green'
        elif j<6:
            return 'red'
        else:
            return 'blue'
    return None

def diffusion_color(i, j, width_len, height_len):
    if i>2*height_len/3:
        if j<4:
            return 'green'
        elif j<10:
            return 'red'
        else:
            return 'blue'
    elif i>height_len/3:
        if j<5:
            return 'green'
        elif j<9:
            return 'red'
        else:
            return 'blue'
    else:
        if j<6:
            return 'green'
        elif j<8:
            return 'red'
        else:
            return 'blue'

width_len = 14
brick_width = 60
width = brick_width*width_len

height_len = 16
brick_height = 20
height = brick_height*height_len

line_thickness = 2

im = Image.new("RGB", (width,height), (255,255,255))

draw = ImageDraw.Draw(im)

y=0
for i in range(0,height_len+1):
    x=0
    for j in range(0,width_len+1):
        color = diffusion_color(i, j, width_len, height_len)

        if i%2==0 and (j==0 or j==width_len):
            draw_rectangle(draw, ((x, y), (x+brick_width/2, y+brick_height)),  outline='black', fill=color, width=line_thickness)
            x+=brick_width/2
        else:
            draw_rectangle(draw, ((x, y), (x+brick_width, y+brick_height)),  outline='black', fill=color, width=line_thickness)
            x+=brick_width
    y+=brick_height
del draw

# write to stdou
im.show()
# im.save('g.png')
