/* OS134, Copyright (C) 2005, Benjamin Stein, Jaap Weel, Ting Liao --
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.  This program is distributed in the
hope that it will be useful, but WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU General Public License for more details.  You
should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA */
        
/*
 * All we're doing here is wrapping around C functions.
 */                  

.macro generic i
        pushal                  /* Push all GPRs. */
        push    $\i             /* Push i. */
        call    generalized_interrupt_handler
        pop     %eax
        popal                   /* Pop all GPRs. */
        addl    $4, %esp        /* Pop the errno. */
        iret
.endm                
        
.macro handler name i
.globl \name
\name:           
        cli                     /* Turn interrupts off. */
        pushl $0                /* Push the errno. */
        generic \i              
.endm

.macro errornr name i
.globl \name
\name:           
        cli                     /* Already pushed errno! */
        generic \i 
.endm                
                     
handler int0, 0
handler int1, 1
handler int2, 2
handler int3, 3
handler int4, 4
handler int5, 5
handler int6, 6
handler int7, 7
errornr int8, 8
handler int9, 9
errornr int10, 10
errornr int11, 11
errornr int12, 12
errornr int13, 13
errornr int14, 14
handler int15, 15
handler int16, 16
handler int17, 17
handler int18, 18
handler int19, 19
handler int20, 20
handler int21, 21
handler int22, 22
handler int23, 23
handler int24, 24
handler int25, 25
handler int26, 26
handler int27, 27
handler int28, 28
handler int29, 29
handler int30, 30
handler int31, 31
handler int32, 32
handler int33, 33
handler int34, 34
handler int35, 35
handler int36, 36
handler int37, 37
handler int38, 38
handler int39, 39
handler int40, 40
handler int41, 41
handler int42, 42
handler int43, 43
handler int44, 44
handler int45, 45
handler int46, 46
handler int47, 47
handler int48, 48
handler int49, 49
handler int50, 50
handler int51, 51
handler int52, 52
handler int53, 53
handler int54, 54
handler int55, 55
handler int56, 56
handler int57, 57
handler int58, 58
handler int59, 59
handler int60, 60
handler int61, 61
handler int62, 62
handler int63, 63
handler int64, 64
handler int65, 65
handler int66, 66
handler int67, 67
handler int68, 68
handler int69, 69
handler int70, 70
handler int71, 71
handler int72, 72
handler int73, 73
handler int74, 74
handler int75, 75
handler int76, 76
handler int77, 77
handler int78, 78
handler int79, 79
handler int80, 80
handler int81, 81
handler int82, 82
handler int83, 83
handler int84, 84
handler int85, 85
handler int86, 86
handler int87, 87
handler int88, 88
handler int89, 89
handler int90, 90
handler int91, 91
handler int92, 92
handler int93, 93
handler int94, 94
handler int95, 95
handler int96, 96
handler int97, 97
handler int98, 98
handler int99, 99
handler int100, 100
handler int101, 101
handler int102, 102
handler int103, 103
handler int104, 104
handler int105, 105
handler int106, 106
handler int107, 107
handler int108, 108
handler int109, 109
handler int110, 110
handler int111, 111
handler int112, 112
handler int113, 113
handler int114, 114
handler int115, 115
handler int116, 116
handler int117, 117
handler int118, 118
handler int119, 119
handler int120, 120
handler int121, 121
handler int122, 122
handler int123, 123
handler int124, 124
handler int125, 125
handler int126, 126
handler int127, 127
handler int128, 128
handler int129, 129
handler int130, 130
handler int131, 131
handler int132, 132
handler int133, 133
handler int134, 134
handler int135, 135
handler int136, 136
handler int137, 137
handler int138, 138
handler int139, 139
handler int140, 140
handler int141, 141
handler int142, 142
handler int143, 143
handler int144, 144
handler int145, 145
handler int146, 146
handler int147, 147
handler int148, 148
handler int149, 149
handler int150, 150
handler int151, 151
handler int152, 152
handler int153, 153
handler int154, 154
handler int155, 155
handler int156, 156
handler int157, 157
handler int158, 158
handler int159, 159
handler int160, 160
handler int161, 161
handler int162, 162
handler int163, 163
handler int164, 164
handler int165, 165
handler int166, 166
handler int167, 167
handler int168, 168
handler int169, 169
handler int170, 170
handler int171, 171
handler int172, 172
handler int173, 173
handler int174, 174
handler int175, 175
handler int176, 176
handler int177, 177
handler int178, 178
handler int179, 179
handler int180, 180
handler int181, 181
handler int182, 182
handler int183, 183
handler int184, 184
handler int185, 185
handler int186, 186
handler int187, 187
handler int188, 188
handler int189, 189
handler int190, 190
handler int191, 191
handler int192, 192
handler int193, 193
handler int194, 194
handler int195, 195
handler int196, 196
handler int197, 197
handler int198, 198
handler int199, 199
handler int200, 200
handler int201, 201
handler int202, 202
handler int203, 203
handler int204, 204
handler int205, 205
handler int206, 206
handler int207, 207
handler int208, 208
handler int209, 209
handler int210, 210
handler int211, 211
handler int212, 212
handler int213, 213
handler int214, 214
handler int215, 215
handler int216, 216
handler int217, 217
handler int218, 218
handler int219, 219
handler int220, 220
handler int221, 221
handler int222, 222
handler int223, 223
handler int224, 224
handler int225, 225
handler int226, 226
handler int227, 227
handler int228, 228
handler int229, 229
handler int230, 230
handler int231, 231
handler int232, 232
handler int233, 233
handler int234, 234
handler int235, 235
handler int236, 236
handler int237, 237
handler int238, 238
handler int239, 239
handler int240, 240
handler int241, 241
handler int242, 242
handler int243, 243
handler int244, 244
handler int245, 245
handler int246, 246
handler int247, 247
handler int248, 248
handler int249, 249
handler int250, 250
handler int251, 251
handler int252, 252
handler int253, 253
handler int254, 254
handler int255, 255
