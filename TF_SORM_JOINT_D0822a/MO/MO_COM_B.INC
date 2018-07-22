MO_0:
MO_1:
        add     bp, 4             ; <- {Paul.N - 08/12/14}
        jmp     SLED_MO
MO_2:
MO_3:
MO_4:
        add     bp, 3           ; ηβ®΅λ ­¥ ΅λ«® ΰ §ΰλΆ  
        jmp     SLED_MO         ;
MO_5:
        add     bp, 4
        jmp     SLED_MO
MO_6:
MO_7:
MO_8:
MO_9:
        add     bp, 3             ; <- {Paul.N - 08/12/14} - ΰ §ΰλΆ ®β –0 Ά “«μο­®Άαª¥
        jmp     SLED_MO
MO_10:
MO_11:
        add     bp, 4
        jmp     SLED_MO
MO_12:
MO_13:
MO_14:
        add     bp, 3
        jmp     SLED_MO
MO_15:  add     bp, ds:[bp + 3] + 4
        JMP     BAD_MO
MO_16:
        add     bp, 4           ; ¨¤¥β ΰ §ΰλΆ  ®β –0 Ά αβ.„¨­αª®©
        jmp     SLED_MO
MO_17:
MO_18:
MO_19:
MO_20:
MO_21:
MO_22:
MO_23:
MO_24:
        add     bp, 3
        jmp     SLED_MO
MO_25:
        add     bp, 4
        jmp     SLED_MO
MO_26:
        add     bp, 3
        jmp     SLED_MO
MO_27:
        add     bp, 1
        jmp     SLED_MO
MO_28:
        add     bp, 4
        jmp     SLED_MO
MO_29:
        add     bp, 24
        jmp     SLED_MO
MO_30:
        add     bp, 21
        jmp     SLED_MO
MO_31:
MO_32:
MO_33:
MO_34:
        add     bp, 1
        jmp     SLED_MO
MO_35:
        add     bp, 5
        jmp     SLED_MO
MO_36:
MO_38:
        add     bp, 2
        jmp     SLED_MO
MO_39:
        add     bp, 6
        jmp     SLED_MO
MO_40:
MO_41:
        add     bp, 3
        jmp     SLED_MO
MO_42:
        add     bp, ds:[bp + 3] + 4
        jmp     SLED_MO
MO_43:
        add     bp, 9
        jmp     SLED_MO
MO_44:
MO_45:
        add     bp, 1
        jmp     SLED_MO
MO_46:
        add     bp, 2
        jmp     SLED_MO
MO_47:
MO_48:
        add     bp, 4
        jmp     SLED_MO
MO_49:
        add     bp, 6
        jmp     SLED_MO
MO_50:
        add     bp, 24
        jmp     SLED_MO
MO_51:
        add     bp, 6
        jmp     SLED_MO
MO_52:
        add     bp, ds:[bp + 6] + 7
        jmp     SLED_MO
MO_53:
MO_54:
        add     bp, 5
        jmp     SLED_MO
MO_55:
        add     bp, 4
        jmp     SLED_MO
MO_56:
        add     bp, 2
        jmp     SLED_MO
MO_57:
        add     bp, 19
        jmp     SLED_MO
MO_58:
        add     bp, 34
        jmp     SLED_MO
MO_59:
        add     bp, 4
        jmp     SLED_MO
MO_60:
        add     bp, 7
        jmp     SLED_MO
MO_61:
        add     bp, 3
        jmp     SLED_MO
                ;;;JMP     BAD_MO

MO_65:
MO_66:
        add     bp, 3
        jmp     SLED_MO
MO_67:
        add     bp, 4
        jmp     SLED_MO
MO_68:
MO_69:
MO_70:
MO_71:
MO_72:
MO_73:
MO_74:
        add     bp, 3
        jmp     SLED_MO
MO_75:
        add     bp, 5
        jmp     SLED_MO
MO_76:
MO_77:
        add     bp, 3
        jmp     SLED_MO
MO_78:
        add     bp, 4
        jmp     SLED_MO
MO_79:
                ; --- ΅λΆθ¨¥ ª®¬ ­¤λ ‘
MO_80:
MO_81:
        add     bp, 3
        jmp     SLED_MO
MO_82:
        add     bp, 5
        jmp     SLED_MO
MO_83:          ; α­οβμ α ­ ΅«ξ¤¥­¨ο
        add     bp, 3
        jmp     SLED_MO
MO_84:          ;‡€‘ ‘‘€ „‚
        add     bp, 44
        jmp     SLED_MO
MO_85:          ;‘’€‚’ € ‘‹“
        add     bp, 15
        jmp     SLED_MO
MO_86:
        add     bp, 2
        jmp     SLED_MO
MO_87:
        add     bp, 4
        jmp     SLED_MO
                ; --- ΅λΆθ¨¥ ª®¬ ­¤λ ‘
MO_88:
MO_89:
        add     bp, 3
        jmp     SLED_MO
MO_90:
        add     bp, 4
        jmp     SLED_MO
MO_91:
MO_92:
        add     bp, 3
        jmp     SLED_MO
MO_93:
        add     bp, ds:[bp + 10] + 11
        jmp     SLED_MO
MO_94:
        add     bp, 3
        jmp     SLED_MO
MO_95:
MO_96:
        add     bp, 4
        jmp     SLED_MO
MO_97:
        add     bp, 44
        jmp     SLED_MO
MO_98:
        add     bp, 7
        jmp     SLED_MO
MO_99:
        add     bp, 3
        jmp     SLED_MO
                ;;JMP     BAD_MO

;  ----------------------------------------------------------------------------
;  --- ’€”€–›… €„› - €—€‹                                     ---
;  ----------------------------------------------------------------------------

; ----= m105 =----------------------------------------------------------------
                        ;0    1     2-3
                        ;COD/ N_TS/ N_FIZ
MO_105:                 ;‡€’… ‹ - ‚•„™…… --> MUST DIE !!!
                ADD     BP,4
                JMP     SLED_MO


; ----= m108 =----------------------------------------------------------------
MO_108:         add     bp ,8
                JMP     BAD_MO

; ----= m111 =----------------------------------------------------------------
                        ;0    1     2-3
                        ;COD/ N_TS/ N_RAB
MO_111:                 ;‚’›‰ ‚›‡‚
                ADD     BP,4
                JMP     SLED_MO

MO_114:                 ;‘‰ ‹
                ADD     BP,5
                JMP     SLED_MO
MO_115:                 ;‡€’… ‘‹ - ‘•„™……
                ADD     BP,4
                JMP     SLED_MO
MO_116:                 ;‘‚†„…… ‘‹
                ADD     BP,4
                JMP     SLED_MO
MO_117:
                JMP     BAD_MO

MO_120:
MO_121:         add     bp, 6        ;…‘€–‚€›‰ „‘’“
                JMP     BAD_MO

;;; ---------------------------------------------------------------------------
;;; --- ’€”€–›… €„› - …–                                     ---
;;; ---------------------------------------------------------------------------



MO_124:
                JMP     BAD_MO

MO_130:         jmp     BAD_MO
MO_131:         add     bp, 7
                JMP     BAD_MO

MO_134:
                JMP     BAD_MO

MO_137:                 ;‚…‘’ ‡…… ‹
MO_138:                 ;‡……… …‚‡†
        add     bp, 5
        jmp     SLED_MO
MO_139:                 ;…‡“‹’€’› ‡…… €‹
        add     bp, 45
        jmp     SLED_MO
MO_140: add     bp, 8   ; ‘’‚‹…… ‹ (‹…‰”€)
                JMP     BAD_MO

MO_142:
        add     bp, 2
        jmp     SLED_MO
MO_143:
MO_144:                 ;„’‚…†„…… „‹.‘‚. N_TEL
        add     bp, 6
        jmp     SLED_MO
MO_145:
MO_146:                 ;„’‚…†„…… €’…ƒ
        add     bp, 5
        jmp     SLED_MO
MO_147:
MO_148:                 ;„’‚…†„…… SPUS
        add     bp, 6
        jmp     SLED_MO
MO_149:
        add     bp, 153
        jmp     SLED_MO
MO_150: add     bp, 3
                JMP     BAD_MO

MO_151:                 ;„’‚…†„…… € ‘‹“‚€…
                ADD     BP,3
                JMP     SLED_MO

; --- ƒ !!!<Yu>
MO_153:         ADD     BP, 4
                JMP     SLED_MO

MO_154:
                JMP     BAD_MO

MO_157:                ; ’€”€–›‰ ‘“‘
                ADD     BP,3
                JMP     SLED_MO

MO_158:
MO_159:
MO_160:
MO_161:
MO_162:
MO_163:
MO_164:
MO_165:
MO_166:
MO_167:
MO_168:
MO_169:
MO_170:
MO_171:
MO_172:
MO_173:
MO_174:
MO_175:
MO_176:
MO_177:
MO_178:
MO_179:
MO_180:
MO_181:
                JMP     BAD_MO


MO_183:
MO_184:
MO_185:
MO_186:
MO_187:
MO_188:
MO_189:
MO_190:
MO_191:
MO_192:
                JMP     BAD_MO

; ----= m193 =----------------------------------------------------------------
MO_193:         ;¨­δ®ΰ¬ ζ¨ο ® α®β®ο­¨¨ ΅«®ª  ¤«ο “
                add     bp, 3
                jmp     SLED_MO

MO_194:
MO_195:
MO_196:
MO_197:
MO_198:
MO_199:
MO_200:
MO_201:
MO_202:
MO_203:
MO_204:
MO_205:
MO_206:
MO_207:
MO_208:
MO_209:
MO_210:
MO_211:
MO_212:
MO_213:
MO_214:
MO_215:
MO_216:
MO_217:
MO_218:
MO_219:
MO_220:
MO_221:                 ;‡…’  …“ ’…‹…”€
MO_222:
MO_223:                 ;‡…’  …“ ”‡—…‘“
MO_224:                 ;…‡“‹’€’› ‡…… €‹
MO_225:                 ;‡…’  …“ ’…‹…”€
MO_226:                 ;‡…’  …“ ”‡—…‘“
MO_227:                 ;‘’‚‹…… ‹ (‹…‰”€)
MO_228:                 ;„€‰ N_FIZ  …“ ’…‹…”€
MO_229:
MO_230:                 ;‘‚„’ … ’…‹…”€
MO_231:                 ;€‡€—’ … ’…‹…”€
MO_232:                 ;„€‰ €’…ƒ  …“ ’…‹…”€
MO_233:
MO_234:                 ;‡…’ €’…ƒ  … ’…‹…”€
                JMP     BAD_MO



MO_236:                 ;‡…’ SPUS  … ’…‹…”€
MO_237:                 ;„€‰ SPUS  …“ ’…‹…”€
MO_238:
MO_239:
MO_240:
MO_241:                 ; €‹
MO_242:
MO_243:
MO_244:
MO_245:
MO_246:
MO_247:
MO_248:
MO_249:
MO_250:
MO_251:
MO_252:
MO_253:
MO_254:
MO_255:
                JMP     BAD_MO




