let _functionContent = {|
open Contract;

open ShaderChunkType;

let _getGLSLChunkMap = (state: StateDataType.state) => state.glslChunkData.chunkMap;

let getChunk = (name: string, state: StateDataType.state) =>
  state |> _getGLSLChunkMap |> HashMapSystem.get(name) |> Js.Option.getExn;

let _buildChunk =
    (
      top: string,
      define: string,
      varDeclare: string,
      funcDeclare: string,
      funcDefine: string,
      body: string
    ) => {
  top,
  define,
  varDeclare,
  funcDeclare,
  funcDefine,
  body
};

let initData = () =>
|};

let _buildInitDataContent = (glslContent: string) => {j|
  HashMapSystem.{
    chunkMap:
      createEmpty()
      $glslContent
  };
|j};

let _buildShunkSystemFileContent = (glslContent) =>
  _functionContent ++ _buildInitDataContent(glslContent);

let _writeToShunkSystemFile = (destFilePath, doneFunc, content) => {
  Node.Fs.writeFileSync(~filename=destFilePath, ~text=content);
  [@bs] doneFunc() |> ignore
};

let _convertArrayToList = (array: array(string)) =>
  array |> Js.Array.reduce((list, str) => [str, ...list], []);

let createShunkSystemFile = (glslPathArr: array(string), destFilePath: string, doneFunc) =>
  glslPathArr
  |> Js.Array.map((glslPath) => Glob.sync(glslPath))
  |> WonderCommonlib.ArraySystem.flatten
  |> _convertArrayToList
  |> List.map(
       (actualGlslPath) =>
         Node.Fs.readFileSync(actualGlslPath, `utf8) |> Parse.parseSegment(actualGlslPath)
     )
  |> Parse.parseImport
  |> _buildShunkSystemFileContent
  |> _writeToShunkSystemFile(destFilePath, doneFunc);