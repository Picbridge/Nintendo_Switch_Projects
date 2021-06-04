#if VERTEX_SHADER
    #define VARYING_QUALIFIER out
    #define VARYING_INSTANCE Out
#endif

#if PIXEL_SHADER
    #define VARYING_QUALIFIER in
    #define VARYING_INSTANCE In
#endif

// vec4[3] * vec4(vec3, 1)
#define TRANSFORM_POS(mtx, pos) vec3( \
    dot(mtx[0].xyzw, pos.xyzw), \
    dot(mtx[1].xyzw, pos.xyzw), \
    dot(mtx[2].xyzw, pos.xyzw))

// vec3[3] * vec3
#define TRANSFORM_VEC(mtx, vec) vec3( \
    dot(mtx[0].xyz, vec.xyz), \
    dot(mtx[1].xyz, vec.xyz), \
    dot(mtx[2].xyz, vec.xyz))

// vec4[3] * vec4(vec3, 1)
#define TRANSFORM_POS_OFFSET(mtx, offset, pos) vec3( \
    dot(mtx[(offset)    ].xyzw, pos.xyzw), \
    dot(mtx[(offset) + 1].xyzw, pos.xyzw), \
    dot(mtx[(offset) + 2].xyzw, pos.xyzw))

// vec3[3] * vec[3]
#define TRANSFORM_VEC_OFFSET(mtx, offset, vec) vec3( \
    dot(mtx[(offset)    ].xyz, vec.xyz), \
    dot(mtx[(offset) + 1].xyz, vec.xyz), \
    dot(mtx[(offset) + 2].xyz, vec.xyz))

// vec4[4] * vec4
#define PROJECT(mtx, pos) vec4( \
    dot(mtx[0].xyzw, pos.xyzw), \
    dot(mtx[1].xyzw, pos.xyzw), \
    dot(mtx[2].xyzw, pos.xyzw), \
    dot(mtx[3].xyzw, pos.xyzw))

// vec4[4] * vec4
#define PROJECT_OFFSET(mtx, offset, pos) vec4( \
    dot(mtx[(offset) + 0], pos), \
    dot(mtx[(offset) + 1], pos), \
    dot(mtx[(offset) + 2], pos), \
    dot(mtx[(offset) + 3], pos))

// vec4[2] * vec2
#define TRANSFORM_UV(mtx, uv) vec2( \
    mtx[0].xy * uv.x + mtx[0].zw * uv.y + mtx[1].xy);

#define TO_BOOL(value) ((value) != 0)

#define  PROJECTION_DIR(x) (-1.0 * (x))



VARYING_QUALIFIER Varying
{
    vec2 texCoord;
    vec3 normal;
} VARYING_INSTANCE;

/// ================================================================
/// 頂点シェーダーの実装
/// ================================================================
#if VERTEX_SHADER
out gl_PerVertex
{
    vec4 gl_Position;
};

layout( location = 0 ) in vec3 i_Position;
layout( location = 1 ) in vec2 i_TexCoord;
layout( location = 2 ) in vec3 i_Normal;

layout(std140) uniform u_Matrix
{
    vec4 mv[3];
    vec4 proj[4];
};

void main()
{
    /// ビュー行列に変換
    vec3 vPos = TRANSFORM_POS(mv,vec4(i_Position,1.0f));
    gl_Position = PROJECT(proj,vec4(vPos,1.0f));
    Out.texCoord = i_TexCoord.xy;
    Out.normal = TRANSFORM_VEC(mv, i_Normal);

#if defined(USE_DEBUG)
    gl_Position = vec4(i_Position,1.0f);
#endif
    
}

#endif

/// ================================================================
/// ピクセルシェーダーの実装
/// ================================================================
#if PIXEL_SHADER

layout( location = 0 ) out vec4 o_Color;

layout( std140 ) uniform u_Modulate
{
    int  modulate;
    vec3 constColor[3];
    vec3 lightDirInView;
};


uniform sampler2D s_Tex;

/// 変調モード
#define MODULATE_TYPE_CONSTANT        ( 0 )
#define MODULATE_TYPE_TEXTRUE         ( 1 )
#define MODULATE_TYPE_LAYERED         ( 2 )
#define MODULATE_TYPE_ALPHA           ( 3 )
#define MODULATE_TYPE_ALPHA_OPA       ( 4 )
#define MODULATE_TYPE_GLASS           ( 5 )

vec4 GetAlbedo()
{
    vec4 texel = texture(s_Tex,In.texCoord);
    vec4 albedo;
    switch(modulate)
    {
    case MODULATE_TYPE_CONSTANT:
        albedo = vec4(constColor[0],1.0f);
        break;
    case MODULATE_TYPE_TEXTRUE:
        albedo = texel;
        break;
    case MODULATE_TYPE_LAYERED:
        albedo = vec4(constColor[0] * texel.r
            + constColor[1] * texel.g
            + constColor[2] * texel.b
            , texel.a);
        break;
    case MODULATE_TYPE_ALPHA:
        albedo = vec4(constColor[0],texel.r);
        break;
    case MODULATE_TYPE_ALPHA_OPA:
        albedo = vec4(constColor[0] * texel.r ,1.0f);
        break;
    case MODULATE_TYPE_GLASS:
        albedo = vec4(constColor[0] * texel.r , pow(texel.g, constColor[1].g));
        break;
    default:
        albedo = vec4(1.0f);
        break;
    }
    return albedo;
}

void main()
{
    /// ModulateTypeを考慮してアルベドを取得
    vec4 albedo = GetAlbedo();
    if(albedo.a == 0.0f)
    {
        discard;
    }

    /// Lambert ライティング
    vec3 normal = normalize(In.normal); ///< ビュー空間法線
    vec3 lightDir = normalize(lightDirInView.xyz); ///< ビュー空間ライト方向
    float diffuseFactor = max(0.0f, dot(normal, lightDir));
    vec4 outputColor = vec4(vec3(albedo.rgb) * diffuseFactor, albedo.a);
    o_Color = outputColor;
}

#endif
