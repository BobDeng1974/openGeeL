
width 	= 1920
height 	= 1080
name    = "DeerScene"
mode 	= "Windowed"

usePhysics  = false
useEmissive = false
exposure = 1

camera = {
	position = { x = -0.03,  y = 0.17, z = 2.66 },
	rotation = { x = -91.59, y = 2.78, z = -3 },

	fov = 25,
	near = 0.01,
	far = 100,

	movable = true,
	speed = 1,
	sensitivity = 0.45
}

skybox = {
	path = "resources/envmaps/WinterForest_Ref.hdr",
	resolution = 1024,

	useIBL = true,
	drawIrradiance = false
}

reflectionprobes = {
	
	{
		position 	= { x = -0.13, 	y = 0.13, 	 z = 0.52 },
		rotation 	= { x = 0,  	y = 0,   	 z = 0 },
		dimensions 	= { width = 20, height = 20, depth = 20},

		resolution = 1024
	},

}


objects = {
	
	{
		position = { x = 0, 	y = 0, 	 z = 0   },
		rotation = { x = 0,  	y = 0,   z = 0   },
		scale 	 = { x = 0.1, 	y = 0.1, z = 0.1 },

		path = "resources/deer/scene2.obj",
		name = "Deer",
		separate = false,

		allmaterials = {
			--roughness = 0,
			metallic = 0.2,
			--transparency = 0,
		},

		materials = {

			{
				name = "Grass",
				roughness = 0.2,

				textures = {
					--[[
					{
						type = "specular",
						path = "resources/girl/cloth_spec_01.jpg"

					}
					--]]
				}
			}

		},

		meshes = {
			--[[
			{
				name   = "defaultobject",
				mask   = "Skin",
				method = "TransparentOD"
			},
			--]]
		},

		scripts = {
			--[[			
			{
				path = "main/deerconfig.lua",
				parameters = {

					{
						name  = "i",
						value = 5
					}
				}
			}
			--]]
		}

	},

}


lights = {
	
	{
		type = "Point",

		position = { x = -0.29, y = 0.39, z = 1.8 },
		rotation = { x = -180, 	y = 0, 	  z = -50},
		color	 = { r = 0.996, g = 0.535,b = 0.379},

		intensity = 7,

		useShadowmap = true,
		shadowBias = 0.00001,
		shadowResolution = "Huge",
		shadowmapType = "Soft",
		softShadowResolution = 15
	}

}


ibl = {}

ssao = {
	radius = 0.5,
	blurSigma = 4,
	blurFactor = 0.291,
	resolution = 0.5
}

ssrr = {
	stepCount = 75,
	stepSize = 0.01,
	stepGain = 1.02,

	blurSigma = 1.3,
	resolution = 0.66
}

dof = {
	aperture = 45,
	focalLength = 2,
	blurSigma = 10,
	resolution = 0.4
}

--[[

colorcorrect = {
	r = 0.5,
	g = 0.3,
	b = 0.9,

	hue 	   = 1,
	saturation = 1,
	brightness = 1,

	--Chromatic aberration
	cr = 0.1,
	cg = 0,
	cb = 0
}

godray = {
	position = { x = -40, y = 30, z = -50 },

	sampleCount = 20,
	blurSigma = 4,
	blurFactor = 0.291,
	resolution = 0.5
}

bloom = {
	scatter = 0.5,
	blurSigma = 3,
	resolution = 0.5
}

--]]

fxaa = {
	minColorDiff = 0,
	fxaaMul = 0
}


gui = {
	showObjects = true,
	showEffects = true,
	showSystem  = true
}
