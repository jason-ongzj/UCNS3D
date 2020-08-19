#--------------------------------------------------------------
# Catalyst pipeline code for coarse TGV

# Global timestep output options
timeStepToStartOutputAt=0
forceOutputAtFirstCall=False

# Global screenshot output options
imageFileNamePadding=0
rescale_lookuptable=False

# Whether or not to request specific arrays from the adaptor.
requestSpecificArrays=False

# a root directory under which all Catalyst output goes
rootDirectory=''

# makes a cinema D index table
make_cinema_table=False

#--------------------------------------------------------------
# Code generated from cpstate.py to create the CoProcessor.
# paraview version 5.8.0
#--------------------------------------------------------------

from paraview.simple import *
from paraview import coprocessing

# ----------------------- CoProcessor definition -----------------------

def CreateCoProcessor():
  def _CreatePipeline(coprocessor, datadescription):
    class Pipeline:
      # state file generated using paraview version 5.8.0

      # ----------------------------------------------------------------
      # setup views used in the visualization
      # ----------------------------------------------------------------

      # trace generated using paraview version 5.8.0
      #
      # To ensure correct image size when batch processing, please search
      # for and uncomment the line `# renderView*.ViewSize = [*,*]`

      #### disable automatic camera reset on 'Show'
      paraview.simple._DisableFirstRenderCameraReset()

      # Create a new 'Render View'
      renderView1 = CreateView('RenderView')
      renderView1.ViewSize = [1373, 799]
      renderView1.AxesGrid = 'GridAxes3DActor'
      renderView1.CenterOfRotation = [3.1415927410125732, 3.1415927410125732, 3.1415927410125732]
      renderView1.StereoType = 'Crystal Eyes'
      renderView1.CameraPosition = [24.47761460639957, 12.689364591769882, 23.182617843014217]
      renderView1.CameraFocalPoint = [3.2426219969131345, 2.4513437939520846, 3.390245478678071]
      renderView1.CameraViewUp = [-0.2652742684092946, 0.9425703651513748, -0.2029548453679232]
      renderView1.CameraFocalDisk = 1.0
      renderView1.CameraParallelScale = 5.44139824412335

      # register the view with coprocessor
      # and provide it with information such as the filename to use,
      # how frequently to write the images, etc.
      coprocessor.RegisterView(renderView1,
          filename='RenderView1_%t.png', freq=10, fittoscreen=0, magnification=1, width=1373, height=799, cinema={}, compression=5)
      renderView1.ViewTime = datadescription.GetTime()

      SetActiveView(None)

      # ----------------------------------------------------------------
      # setup view layouts
      # ----------------------------------------------------------------

      # create new layout object 'Layout #1'
      layout1 = CreateLayout(name='Layout #1')
      layout1.AssignView(0, renderView1)

      # ----------------------------------------------------------------
      # restore active view
      SetActiveView(renderView1)
      # ----------------------------------------------------------------

      # ----------------------------------------------------------------
      # setup the data processing pipelines
      # ----------------------------------------------------------------

      # create a new 'Text'
      text1 = Text()
      text1.Text = 'UCNS3D-Catalyst'

      # create a new 'PVTrivialProducer'
      # create a producer from a simulation input
      input = coprocessor.CreateProducer(datadescription, 'input')

      # create a new 'Threshold'
      threshold1 = Threshold(Input=input)
      threshold1.Scalars = ['CELLS', 'Q']
      threshold1.ThresholdRange = [0.1, 0.5]

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView1'
      # ----------------------------------------------------------------

      # show data from threshold1
      threshold1Display = Show(threshold1, renderView1, 'UnstructuredGridRepresentation')

      # get color transfer function/color map for 'Q'
      qLUT = GetColorTransferFunction('Q')
      qLUT.RGBPoints = [0.10000495734094464, 0.831373, 0.909804, 0.980392, 0.10500309553595741, 0.74902, 0.862745, 0.960784, 0.11000123373097019, 0.694118, 0.827451, 0.941176, 0.11999751012099573, 0.568627, 0.760784, 0.921569, 0.12999378651102128, 0.45098, 0.705882, 0.901961, 0.13999006290104682, 0.345098, 0.643137, 0.858824, 0.14998633929107236, 0.247059, 0.572549, 0.819608, 0.15998261568109792, 0.180392, 0.521569, 0.780392, 0.16398112623710814, 0.14902, 0.490196, 0.74902, 0.17197814734912859, 0.129412, 0.447059, 0.709804, 0.17997516846114903, 0.101961, 0.427451, 0.690196, 0.18397367901715922, 0.094118, 0.403922, 0.658824, 0.18797218957316947, 0.090196, 0.392157, 0.639216, 0.19197070012917966, 0.082353, 0.368627, 0.619608, 0.19596921068518988, 0.070588, 0.352941, 0.6, 0.1999677212412001, 0.066667, 0.329412, 0.568627, 0.20396623179721032, 0.07451, 0.313725, 0.541176, 0.20796474235322054, 0.086275, 0.305882, 0.509804, 0.21196325290923076, 0.094118, 0.286275, 0.478431, 0.21596176346524099, 0.101961, 0.278431, 0.45098, 0.21996027402125118, 0.109804, 0.266667, 0.411765, 0.22395878457726143, 0.113725, 0.258824, 0.380392, 0.22795729513327165, 0.113725, 0.25098, 0.34902, 0.23195580568928187, 0.109804, 0.266667, 0.321569, 0.2359543162452921, 0.105882, 0.301961, 0.262745, 0.23995282680130228, 0.094118, 0.309804, 0.243137, 0.2439513373573125, 0.082353, 0.321569, 0.227451, 0.24794984791332272, 0.07451, 0.341176, 0.219608, 0.25194835846933294, 0.070588, 0.360784, 0.211765, 0.25594686902534314, 0.066667, 0.380392, 0.215686, 0.2599453795813534, 0.062745, 0.4, 0.176471, 0.2699416559713789, 0.07451, 0.419608, 0.145098, 0.2799379323614045, 0.086275, 0.439216, 0.117647, 0.28993420875143, 0.121569, 0.470588, 0.117647, 0.2999304851414556, 0.184314, 0.501961, 0.14902, 0.30992676153148113, 0.254902, 0.541176, 0.188235, 0.31992303792150667, 0.32549, 0.580392, 0.231373, 0.3299193143115322, 0.403922, 0.619608, 0.278431, 0.33991559070155775, 0.501961, 0.670588, 0.333333, 0.35191112236958844, 0.592157, 0.729412, 0.4, 0.3599081434816088, 0.741176, 0.788235, 0.490196, 0.3679051645936293, 0.858824, 0.858824, 0.603922, 0.3799006962616599, 0.921569, 0.835294, 0.580392, 0.399893249041711, 0.901961, 0.729412, 0.494118, 0.41988580182176216, 0.858824, 0.584314, 0.388235, 0.43987835460181324, 0.8, 0.439216, 0.321569, 0.4598709073818643, 0.678431, 0.298039, 0.203922, 0.4798634601619154, 0.54902, 0.168627, 0.109804, 0.4898597365519409, 0.478431, 0.082353, 0.047059, 0.49985601294196647, 0.45098, 0.007843, 0.0]
      qLUT.ColorSpace = 'RGB'
      qLUT.NanColor = [0.25, 0.0, 0.0]
      qLUT.ScalarRangeInitialized = 1.0

      # get opacity transfer function/opacity map for 'Q'
      qPWF = GetOpacityTransferFunction('Q')
      qPWF.Points = [0.10000495734094464, 0.0, 0.5, 0.0, 0.49985601294196647, 1.0, 0.5, 0.0]
      qPWF.ScalarRangeInitialized = 1

      # trace defaults for the display properties.
      threshold1Display.Representation = 'Surface'
      threshold1Display.ColorArrayName = ['CELLS', 'Q']
      threshold1Display.LookupTable = qLUT
      threshold1Display.OSPRayScaleArray = 'Q'
      threshold1Display.OSPRayScaleFunction = 'PiecewiseFunction'
      threshold1Display.SelectOrientationVectors = 'None'
      threshold1Display.ScaleFactor = 0.6283185482025146
      threshold1Display.SelectScaleArray = 'None'
      threshold1Display.GlyphType = 'Arrow'
      threshold1Display.GlyphTableIndexArray = 'None'
      threshold1Display.GaussianRadius = 0.031415927410125735
      threshold1Display.SetScaleArray = ['POINTS', 'Q']
      threshold1Display.ScaleTransferFunction = 'PiecewiseFunction'
      threshold1Display.OpacityArray = ['POINTS', 'Q']
      threshold1Display.OpacityTransferFunction = 'PiecewiseFunction'
      threshold1Display.DataAxesGrid = 'GridAxesRepresentation'
      threshold1Display.PolarAxes = 'PolarAxesRepresentation'
      threshold1Display.ScalarOpacityFunction = qPWF
      threshold1Display.ScalarOpacityUnitDistance = 0.5409136231413432

      # init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
      threshold1Display.ScaleTransferFunction.Points = [-0.8807195516841666, 0.0, 0.5, 0.0, 0.8622516214808866, 1.0, 0.5, 0.0]

      # init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
      threshold1Display.OpacityTransferFunction.Points = [-0.8807195516841666, 0.0, 0.5, 0.0, 0.8622516214808866, 1.0, 0.5, 0.0]

      # show data from text
      text1Display = Show(text1, renderView1, 'TextSourceRepresentation')

      # trace defaults for the display properties.
      text1Display.FontFamily = 'Times'
      text1Display.Bold = 1
      text1Display.WindowLocation = 'AnyLocation'
      text1Display.Position = [0.03840495265841223, 0.868597857321652]

      # setup the color legend parameters for each legend in this view

      # get color legend/bar for qLUT in view renderView1
      qLUTColorBar = GetScalarBar(qLUT, renderView1)
      qLUTColorBar.AutoOrient = 0
      qLUTColorBar.Orientation = 'Horizontal'
      qLUTColorBar.WindowLocation = 'AnyLocation'
      qLUTColorBar.Position = [0.25782957028404957, 0.15062578031804894]
      qLUTColorBar.Title = 'Q-Criterion'
      qLUTColorBar.ComponentTitle = ''
      qLUTColorBar.TitleBold = 1
      qLUTColorBar.TitleFontSize = 20
      qLUTColorBar.TextPosition = 'Ticks left/bottom, annotations right/top'
      qLUTColorBar.ScalarBarLength = 0.5000000000000004

      # set color bar visibility
      qLUTColorBar.Visibility = 1

      # show color legend
      threshold1Display.SetScalarBarVisibility(renderView1, True)

      # ----------------------------------------------------------------
      # setup color maps and opacity mapes used in the visualization
      # note: the Get..() functions create a new object, if needed
      # ----------------------------------------------------------------

      # ----------------------------------------------------------------
      # finally, restore active source
      SetActiveSource(text1)
      # ----------------------------------------------------------------
    return Pipeline()

  class CoProcessor(coprocessing.CoProcessor):
    def CreatePipeline(self, datadescription):
      self.Pipeline = _CreatePipeline(self, datadescription)

  coprocessor = CoProcessor()
  # these are the frequencies at which the coprocessor updates.
  freqs = {'input': [10]}
  coprocessor.SetUpdateFrequencies(freqs)
  if requestSpecificArrays:
    arrays = [['Q', 0], ['RU', 0], ['RV', 0], ['Q', 1], ['RU', 1], ['RV', 1]]
    coprocessor.SetRequestedArrays('input', arrays)
  coprocessor.SetInitialOutputOptions(timeStepToStartOutputAt,forceOutputAtFirstCall)

  if rootDirectory:
      coprocessor.SetRootDirectory(rootDirectory)

  if make_cinema_table:
      coprocessor.EnableCinemaDTable()

  return coprocessor


#--------------------------------------------------------------
# Global variable that will hold the pipeline for each timestep
# Creating the CoProcessor object, doesn't actually create the ParaView pipeline.
# It will be automatically setup when coprocessor.UpdateProducers() is called the
# first time.
coprocessor = CreateCoProcessor()

#--------------------------------------------------------------
# Enable Live-Visualizaton with ParaView and the update frequency
coprocessor.EnableLiveVisualization(True, 1)

# ---------------------- Data Selection method ----------------------

def RequestDataDescription(datadescription):
    "Callback to populate the request for current timestep"
    global coprocessor

    # setup requests for all inputs based on the requirements of the
    # pipeline.
    coprocessor.LoadRequestedData(datadescription)

# ------------------------ Processing method ------------------------

def DoCoProcessing(datadescription):
    "Callback to do co-processing for current timestep"
    global coprocessor

    # Update the coprocessor by providing it the newly generated simulation data.
    # If the pipeline hasn't been setup yet, this will setup the pipeline.
    coprocessor.UpdateProducers(datadescription)

    # Write output data, if appropriate.
    coprocessor.WriteData(datadescription);

    # Write image capture (Last arg: rescale lookup table), if appropriate.
    coprocessor.WriteImages(datadescription, rescale_lookuptable=rescale_lookuptable,
        image_quality=0, padding_amount=imageFileNamePadding)

    # Live Visualization, if enabled.
    coprocessor.DoLiveVisualization(datadescription, "localhost", 22222)
